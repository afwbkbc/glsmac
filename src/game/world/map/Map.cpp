#include "Map.h"

#include "engine/Engine.h"

#include "types/Mesh.h"

namespace game {
namespace world {
namespace map {

#define MAP_POSITION { 0.0, 0.0, 0.0 }
#define MAP_ROTATION { 0.0, 0.0, 0.0 }

#define TILE_WIDTH 1.0f
#define TILE_HEIGHT 1.0f
#define TILE_Z_SCALE 1.0f

// must match PCX
#define TEXTURE_WIDTH 56
#define TEXTURE_HEIGHT 56

#define TILE_HALFWIDTH ( TILE_WIDTH / 2 )
#define TILE_HALFHEIGHT ( TILE_HEIGHT / 2 )
#define TEXTURE_HALFWIDTH ( TEXTURE_WIDTH / 2 )
#define TEXTURE_HALFHEIGHT ( TEXTURE_HEIGHT / 2 )

#define B(x) S_to_binary_(#x)

static inline unsigned char S_to_binary_(const char *s)
{
    unsigned long long i = 0;
    while (*s) {
        i <<= 1;
        i += *s++ - '0';
    }
    return i;
}

Map::Map( Scene* scene )
	: m_scene( scene )
{
	
	// precache tile texture variants/rotations for fast lookups
	typedef struct {
		uint8_t bitmask;
		uint8_t checked_bits; // some masks don't care about some corners
	} texture_rule_t;
	
	const vector< texture_rule_t > bitmasks = {
		{ B(00000000), B(10101010) }, // 0
		{ B(00001000), B(10101010) }, // 1
		{ B(00101000), B(10111010) }, // 2
		{ B(00111000), B(10111010) }, // 3
		{ B(10001000), B(10101010) }, // 4
		{ B(10101000), B(11111010) }, // 5
		{ B(10111000), B(11111010) }, // 6
		{ B(11101000), B(11111010) }, // 7
		{ B(11111000), B(11111010) }, // 8
		{ B(10101010), B(11111111) }, // 9
		{ B(10101110), B(11111111) }, // 10
		{ B(10111110), B(11111111) }, // 11
		{ B(11101110), B(11111111) }, // 12
		{ B(11111110), B(11111111) }, // 13
		{ B(11111111), B(11111111) }, // 14
		// 15 ???
	};

	for ( uint16_t bitmask = 0 ; bitmask < 256 ; bitmask++ ) {
		ssize_t i = bitmasks.size() - 1;
		while ( i >= 0 ) {
			texture_rule_t rules = bitmasks.at( i );
			if ( ( bitmask & rules.checked_bits ) == ( rules.bitmask & rules.checked_bits ) ) {
				m_texture_variants[ bitmask ] = i;
				break;
			}
			i--;
		}
		if ( i == 0 ) {
			m_texture_variants[ bitmask ] = 0;
		}
	}
	
	
}

Map::~Map() {
	if ( m_actors.terrain ) {
		m_scene->RemoveActor( m_actors.terrain );
		DELETE( m_actors.terrain );
	}
	if ( m_tiles ) {
		DELETE( m_tiles );
	}
	if ( m_textures.terrain ) {
		DELETE( m_textures.terrain );
	}
}

void Map::SetTiles( Tiles* tiles ) {
	ASSERT( !m_tiles, "map tiles already set" );
	m_tiles = tiles;
	GenerateActors();
}

vector<actor::Mesh*> Map::GetActors() const {
	return {
		m_actors.terrain,
	};
}

void Map::GenerateActors() {
	ASSERT( m_tiles, "map tiles not set" );
	m_tiles->Validate();
	
	size_t w = m_tiles->GetWidth();
	size_t h = m_tiles->GetHeight();

	Log( "Loading map" );
	
	if ( !m_textures.source ) {
		m_textures.source = g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", Color::RGBA( 125, 0, 128, 255 ) );
	}
	if ( m_actors.terrain ) {
		m_scene->RemoveActor( m_actors.terrain );
		DELETE( m_actors.terrain );
	}
	if ( m_textures.terrain ) {
		DELETE( m_textures.terrain );
	}
	
	// offsets will align on Y axis
	
	const size_t o_land = 0; // land tiles
	const size_t o_water_surface = 1; // water tiles (alpha surface with gradients)
	const size_t o_water = 2; // sea (for non-alpha objects on water)
	
	const size_t o_max = 3;

	types::Mesh::index_t center, left, right, top, bottom;
	
	NEW( m_textures.terrain, Texture, "TerrainTexture", w * TEXTURE_WIDTH, ( h * o_max ) * TEXTURE_HEIGHT );
	NEWV( mesh_terrain, types::Mesh, w * ( h * o_max ) * 5 / 2, w * ( h * o_max ) * 4 / 2 );
	
	float xpos, ypos;
	float tx, ty, tx1, ty1, tx2, ty2;
	
	float ox = -( (float) TILE_WIDTH * m_tiles->GetWidth() / 4 - TILE_HALFWIDTH );
	float oy = -( (float) TILE_HEIGHT * m_tiles->GetHeight() / 4 - TILE_HALFHEIGHT );
	
	util::Clamper< float > elevation_to_vertex_z( Tile::ELEVATION_MIN, Tile::ELEVATION_MAX, -TILE_Z_SCALE, TILE_Z_SCALE );
	util::Clamper< float > elevation_to_water_r( Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.0f, 0.4f );
	util::Clamper< float > elevation_to_water_g( Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.2f, 0.9f );
	util::Clamper< float > elevation_to_water_b( Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.3f, 1.0f );
	util::Clamper< float > elevation_to_water_a( Tile::ELEVATION_LEVEL_OCEAN, Tile::ELEVATION_LEVEL_COAST, 0.95f, 0.7f );
	
	Tile* tile;
	
	float tsx = (float) 1 / TEXTURE_WIDTH / w;
	float tsy = (float) 1 / TEXTURE_HEIGHT / h / o_max;
	
	// small padding to remove border artifacts (which look like tile border) around tiles
	// proper tile border will be drawn later
	float tsp = 1;
	
	float ttx1, tty1, ttx2 ,tty2;
	Vec2< float > txvec[ 5 ]; // center, left, top, right, bottom
	
	for ( size_t y = 0 ; y < h ; y++ ) {
		for ( size_t x = 0 ; x < w ; x++ ) {
			if ( y % 2 != x % 2 ) {
				continue;
			}
			
			tile = m_tiles->At( x, y );
			
			xpos = ox + TILE_HALFWIDTH * x;
			ypos = oy + TILE_HALFHEIGHT * y;
			
			tx1 = x * TEXTURE_WIDTH;
			ty1 = y * TEXTURE_HEIGHT;
			tx2 = tx1 + TEXTURE_WIDTH;
			ty2 = ty1 + TEXTURE_HEIGHT;
			tx = tx1 + TEXTURE_HALFWIDTH;
			ty = ty1 + TEXTURE_HALFHEIGHT;
			
			auto tx_add = [ this, tx1, ty1, h ] ( const size_t offset, const tc_t& tc, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha = 1.0f ) -> void {
				m_textures.terrain->AddFrom( m_textures.source, mode, tc.x, tc.y, tc.x + TEXTURE_WIDTH - 1, tc.y + TEXTURE_HEIGHT - 1, tx1, offset * h * TEXTURE_HEIGHT + ty1, rotate, alpha );
			};
			
			tile_texture_info_t txinfo;
			txinfo.inverse_x = rand() % 2;
			txinfo.inverse_y = rand() % 2;
			txinfo.rotate_direction = rand() % 4;
			txinfo.texture_variant = 0;
			
			Tile::elevation_t e_left = *tile->elevation.left;
			Tile::elevation_t e_top = *tile->elevation.top;
			Tile::elevation_t e_right = *tile->elevation.right;
			Tile::elevation_t e_bottom = *tile->elevation.bottom;
			Tile::elevation_t e_center = *tile->elevation.center;
			
			bool has_water = (
				e_center <= Tile::ELEVATION_LEVEL_COAST ||
				e_left <= Tile::ELEVATION_LEVEL_COAST ||
				e_top <= Tile::ELEVATION_LEVEL_COAST ||
				e_right <= Tile::ELEVATION_LEVEL_COAST ||
				e_bottom <= Tile::ELEVATION_LEVEL_COAST
			);
			
			if ( has_water ) {
				// mixing with coast texture is too much effort, just simulate it with shaders later
				tx_add( o_water_surface, m_tc.water[ 1 ], Texture::AM_COPY, txinfo.rotate_direction );
			}
			
			if ( tile->is_water_tile ) {
				// don't differentiate under water
				tx_add( o_land, m_tc.water[ 1 ], Texture::AM_COPY, txinfo.rotate_direction );
				//tx_add( o_land, m_tc.arid[0], Texture::AM_COPY, txinfo.rotate_direction );
			}
			else {
				switch ( tile->moisture ) {
					case Tile::M_NONE: {
						// invisible tile (for dev/test purposes)
						break;
					}
					case Tile::M_ARID: {
						tx_add( o_land, m_tc.arid[0], Texture::AM_COPY, txinfo.rotate_direction );
						break;
					}
					case Tile::M_MOIST: {
						txinfo = GetTileTextureInfo( tile, TG_MOISTURE );
						tx_add( o_land, m_tc.moist[ txinfo.texture_variant ], Texture::AM_COPY, txinfo.rotate_direction );
						break;
					}
					case Tile::M_RAINY: {
						txinfo = GetTileTextureInfo( tile, TG_MOISTURE );
						tx_add( o_land, m_tc.rainy[ txinfo.texture_variant ], Texture::AM_COPY, txinfo.rotate_direction );
						break;
					}
					default:
						ASSERT( false, "invalid moisture value" );
				}

				switch ( tile->rockyness ) {
					case Tile::R_NONE:
					case Tile::R_FLAT: {
						// nothing
						break;
					}
					case Tile::R_ROLLING: {
						tx_add( o_land, m_tc.rocks[ rand() % 2 * 2 ], Texture::AM_MERGE, txinfo.rotate_direction );
						break;
					}
					case Tile::R_ROCKY: {
						tx_add( o_land, m_tc.rocks[ rand() % 2 * 2 + 1 ], Texture::AM_MERGE, txinfo.rotate_direction );
						break;
					}
					default:
						ASSERT( false, "invalid rockyness value" );
				}
			}

			if ( !tile->is_water_tile ) {
				if ( tile->features	& Tile::F_JUNGLE ) {
					txinfo = GetTileTextureInfo( tile, TG_FEATURE, Tile::F_JUNGLE );
					tx_add( o_land, m_tc.jungle[ txinfo.texture_variant ], Texture::AM_MERGE, txinfo.rotate_direction );
				}
			}
			
			if ( tile->features & Tile::F_XENOFUNGUS ) {
				txinfo = GetTileTextureInfo( tile, TG_FEATURE, Tile::F_XENOFUNGUS );
				if ( tile->is_water_tile ) {
					tx_add( o_water, m_tc.fungus_sea[ txinfo.texture_variant ], Texture::AM_MERGE, txinfo.rotate_direction );
				}
				else {
					tx_add( o_land, m_tc.fungus_land[ txinfo.texture_variant ], Texture::AM_MERGE, txinfo.rotate_direction );
				}
			}
			
			if ( txinfo.inverse_x ) {
				ttx1 = tx2 - tsp;
				ttx2 = tx1 + tsp;
			}
			else {
				ttx1 = tx1 + tsp;
				ttx2 = tx2 - tsp;
			}
			if ( txinfo.inverse_y ) {
				tty1 = ty2 - tsp;
				tty2 = ty1 + tsp;
			}
			else {
				tty1 = ty1 + tsp;
				tty2 = ty2 - tsp;
			}
			
			txinfo.rotate_direction = 0; // don't rotate combined texture, everything in it is already rotated
			txvec[ txinfo.rotate_direction++ ] = { tx, ty };
			txvec[ txinfo.rotate_direction++ ] = { ttx1, tty2 };
			txvec[ txinfo.rotate_direction++ ] = { ttx1, tty1 };
			txvec[ txinfo.rotate_direction++ ] = { ttx2, tty1 };
			txvec[ txinfo.rotate_direction++ ] = { ttx2, tty2 };

			// modify elevations based on water / not water, to avoid displaying half-submerged tiles
			// original tile isn't modified, this is just for rendering
			int8_t em = tile->is_water_tile ? -1 : 3; // setting -1 : 100 gives interesting shadow effect, but it's not very realistic
			if (
				( tile->is_water_tile != tile->W->is_water_tile ) ||
				( tile->is_water_tile != tile->NW->is_water_tile ) ||
				( tile->is_water_tile != tile->SW->is_water_tile )
			) {
				e_left = Tile::ELEVATION_LEVEL_COAST + em;
			}
			if (
				( tile->is_water_tile != tile->E->is_water_tile ) ||
				( tile->is_water_tile != tile->NE->is_water_tile ) ||
				( tile->is_water_tile != tile->SE->is_water_tile )
			) {
				e_right = Tile::ELEVATION_LEVEL_COAST + em;
			}
			if (
				( tile->is_water_tile != tile->N->is_water_tile ) ||
				( tile->is_water_tile != tile->NE->is_water_tile ) ||
				( tile->is_water_tile != tile->NW->is_water_tile )
			) {
				e_top = Tile::ELEVATION_LEVEL_COAST + em;
			}
			if (
				( tile->is_water_tile != tile->S->is_water_tile ) ||
				( tile->is_water_tile != tile->SE->is_water_tile ) ||
				( tile->is_water_tile != tile->SW->is_water_tile )
			) {
				e_bottom = Tile::ELEVATION_LEVEL_COAST + em;
			}
			
			if ( tile->is_water_tile ) {
				// do not allow anything above water on water tiles
				if ( e_left >= Tile::ELEVATION_LEVEL_COAST + em )
					e_left = Tile::ELEVATION_LEVEL_COAST + em;
				if ( e_right >= Tile::ELEVATION_LEVEL_COAST + em )
					e_right = Tile::ELEVATION_LEVEL_COAST + em;
				if ( e_top >= Tile::ELEVATION_LEVEL_COAST + em )
					e_top = Tile::ELEVATION_LEVEL_COAST + em;
				if ( e_bottom >= Tile::ELEVATION_LEVEL_COAST + em )
					e_bottom = Tile::ELEVATION_LEVEL_COAST + em;
			}
			else {
				// do not allow anything below water on land tiles
				if ( e_left <= Tile::ELEVATION_LEVEL_COAST - em )
					e_left = Tile::ELEVATION_LEVEL_COAST - em;
				if ( e_right <= Tile::ELEVATION_LEVEL_COAST - em )
					e_right = Tile::ELEVATION_LEVEL_COAST - em;
				if ( e_top <= Tile::ELEVATION_LEVEL_COAST - em )
					e_top = Tile::ELEVATION_LEVEL_COAST - em;
				if ( e_bottom <= Tile::ELEVATION_LEVEL_COAST - em )
					e_bottom = Tile::ELEVATION_LEVEL_COAST - em;
			}
			
			// average new center from new corners
			e_center = ( e_left + e_top + e_right + e_bottom ) / 4;

			// texture coordinates
			#define txc( _o, _c ) { txvec[ _c ].x * tsx, ( txvec[ _c ].y + _o * h * TEXTURE_HEIGHT ) * tsy }
			
			// land
			{
				center = mesh_terrain->AddVertex( { xpos, ypos, elevation_to_vertex_z.Clamp( e_center ) }, txc( o_land, 0 ) );
				left = mesh_terrain->AddVertex( { xpos - TILE_HALFWIDTH, ypos, elevation_to_vertex_z.Clamp( e_left ) }, txc( o_land, 1 ) );
				top = mesh_terrain->AddVertex( { xpos, ypos - TILE_HALFHEIGHT, elevation_to_vertex_z.Clamp( e_top ) }, txc( o_land, 2 ) );
				right = mesh_terrain->AddVertex( { xpos + TILE_HALFWIDTH, ypos, elevation_to_vertex_z.Clamp( e_right ) }, txc( o_land, 3 ) );
				bottom = mesh_terrain->AddVertex( { xpos, ypos + TILE_HALFHEIGHT, elevation_to_vertex_z.Clamp( e_bottom ) }, txc( o_land, 4 ) );
				mesh_terrain->AddSurface( { center, left, top } );
				mesh_terrain->AddSurface( { center, top, right } );
				mesh_terrain->AddSurface( { center, right, bottom } );
				mesh_terrain->AddSurface( { center, bottom, left } );
			}
			
			float water_z = elevation_to_vertex_z.Clamp( Tile::ELEVATION_LEVEL_COAST ); // sea is always on sea level
				
			// water surface
			{
				#define wr elevation_to_water_r.Clamp
				#define wg elevation_to_water_g.Clamp
				#define wb elevation_to_water_b.Clamp
				#define wa elevation_to_water_a.Clamp
				#define txt( _value ) { wr( _value ), wg( _value ), wb( _value ), wa( _value ) }  // tint
				center = mesh_terrain->AddVertex( { xpos, ypos, water_z }, txc( o_water_surface, 0 ), txt( e_center) );
				left = mesh_terrain->AddVertex( { xpos - TILE_HALFWIDTH, ypos, water_z }, txc( o_water_surface, 1 ), txt( e_left ) );
				top = mesh_terrain->AddVertex( { xpos, ypos - TILE_HALFHEIGHT, water_z }, txc( o_water_surface, 2 ), txt( e_top ) );
				right = mesh_terrain->AddVertex( { xpos + TILE_HALFWIDTH, ypos, water_z }, txc( o_water_surface, 3 ), txt( e_right ) );
				bottom = mesh_terrain->AddVertex( { xpos, ypos + TILE_HALFHEIGHT, water_z }, txc( o_water_surface, 4 ), txt( e_bottom ) );
				mesh_terrain->AddSurface( { center, left, top } );
				mesh_terrain->AddSurface( { center, top, right } );
				mesh_terrain->AddSurface( { center, right, bottom } );
				mesh_terrain->AddSurface( { center, bottom, left } );
			}
			
			// water
			{
				center = mesh_terrain->AddVertex( { xpos, ypos, water_z }, txc( o_water, 0 ) );
				left = mesh_terrain->AddVertex( { xpos - TILE_HALFWIDTH, ypos, water_z }, txc( o_water, 1 ) );
				top = mesh_terrain->AddVertex( { xpos, ypos - TILE_HALFHEIGHT, water_z }, txc( o_water, 2 ) );
				right = mesh_terrain->AddVertex( { xpos + TILE_HALFWIDTH, ypos, water_z }, txc( o_water, 3 ) );
				bottom = mesh_terrain->AddVertex( { xpos, ypos + TILE_HALFHEIGHT, water_z }, txc( o_water, 4 ) );
				mesh_terrain->AddSurface( { center, left, top } );
				mesh_terrain->AddSurface( { center, top, right } );
				mesh_terrain->AddSurface( { center, right, bottom } );
				mesh_terrain->AddSurface( { center, bottom, left } );
			}
		}
	}
	
	mesh_terrain->Finalize();
	NEW( m_actors.terrain, actor::Mesh, "MapTerrain", mesh_terrain );
		m_actors.terrain->SetTexture( m_textures.terrain );
		m_actors.terrain->SetPosition( MAP_POSITION );
		m_actors.terrain->SetAngle( MAP_ROTATION );
	m_scene->AddActor( m_actors.terrain );
}

const Map::tile_texture_info_t Map::GetTileTextureInfo( Tile* tile, const tile_grouping_criteria_t criteria, const Tile::feature_t feature ) const {
	Map::tile_texture_info_t info;
	
	info.inverse_x = 0;
	info.inverse_y = 0;
	
	bool matches[ 16 ];
	uint8_t idx = 0;
	for ( uint8_t i = 0 ; i < 2 ; i++ ) {
		for ( auto& t : tile->neighbours ) {
			switch ( criteria ) {
				case TG_MOISTURE: {
					matches[ idx++ ] = t->moisture == tile->moisture;
					break;
				}
				case TG_FEATURE: {
					matches[ idx++ ] =
						//( t->is_water_tile == tile->is_water_tile ) && // should land fungus connect to sea fungus? probably.
						( t->features & feature ) == ( tile->features & feature )
					;
					break;
				}
				default: {
					ASSERT( false, "invalid tile grouping criteria" );
				}
			}
		}
	}
	
	vector< uint8_t > possible_rotates = {};
	
	for ( info.rotate_direction = 0 ; info.rotate_direction < 8 ; info.rotate_direction += 2 ) {
		uint8_t bitmask = 0;
		for ( uint8_t i = 0 ; i < 8 ; i++ ) {
			bitmask |= matches[ i + info.rotate_direction ] << i;
		}
		auto it = m_texture_variants.find( bitmask );
		if ( it != m_texture_variants.end() ) {
			info.texture_variant = it->second;
			possible_rotates.push_back( info.rotate_direction );
		}
	}
	
	if ( !possible_rotates.empty() ) {
		info.rotate_direction = possible_rotates[ rand() % possible_rotates.size() ] / 2; // TODO: rotate in all 8 directions?
	}
	else {
		ASSERT( false, "could not find texture variant" );
	}
	
	return info;
}

}
}
}
