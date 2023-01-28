#include "Map.h"

#include "engine/Engine.h"

#include "scene/mesh/Mesh.h"

namespace game {
namespace world {
namespace map {

#define MAP_POSITION { 0.0, 0.0, 0.0 }
#define MAP_ROTATION { 0.0, 0.0, 0.0 }

#define TILE_WIDTH 1.0f
#define TILE_HEIGHT 1.0f
#define TILE_Z_SCALE 0.5f

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
	if ( m_terrain_actor ) {
		DELETE( m_terrain_actor );
	}
	if ( m_tiles ) {
		DELETE( m_tiles );
	}
	if ( m_texture ) {
		DELETE( m_texture );
	}
}

void Map::SetTiles( Tiles* tiles ) {
	ASSERT( !m_tiles, "map tiles already set" );
	m_tiles = tiles;
	GenerateActor();
}

actor::Mesh* Map::GetActor() const {
	return m_terrain_actor;
}

void Map::GenerateActor() {
	ASSERT( m_tiles, "map tiles not set" );
	
	m_tiles->Validate();
	
	if ( m_terrain_actor ) {
		m_scene->RemoveActor( m_terrain_actor );
		DELETE( m_terrain_actor );
		Log( "Regenerating terrain actor" );
	}
	else {
		Log( "Generating terrain actor" );
	}
	
	if ( !m_source_texture ) {
		m_source_texture = g_engine->GetTextureLoader()->LoadTexture( "texture.pcx", Color::RGBA( 125, 0, 128, 255 ) );
	}
	
	size_t w = m_tiles->GetWidth();
	size_t h = m_tiles->GetHeight();
	
	if ( m_texture ) {
		DELETE( m_texture );
	}
	NEW( m_texture, Texture, "MapTexture", w * TEXTURE_WIDTH, h * TEXTURE_HEIGHT );
	
	mesh::Mesh::index_t center, left, right, top, bottom;
	float xpos, ypos;
	float tx, ty, tx1, ty1, tx2, ty2;
	
	float ox = -( (float) TILE_WIDTH * m_tiles->GetWidth() / 4 - TILE_HALFWIDTH );
	float oy = - ( (float) TILE_HEIGHT * m_tiles->GetHeight() / 4 - TILE_HALFHEIGHT );
	
	util::Clamper< float > zc;
	zc.SetRange( Tile::ELEVATION_MIN, Tile::ELEVATION_MAX, -TILE_Z_SCALE, TILE_Z_SCALE );
	
	Tile* tile;
	
	float tsx = (float) 1 / TEXTURE_WIDTH / w;
	float tsy = (float) 1 / TEXTURE_HEIGHT / h;
	float tso = 0; // small offset to remove border artifacts around tiles
	
	float ttx1, tty1, ttx2 ,tty2;
	Vec2< float > txvec[4];
	
	NEWV( mesh, mesh::Mesh, w * h * 5 / 2, w * h * 4 / 2 );
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
			
			auto tx_add = [ this, tx1, ty1 ] ( const tc_t& tc, const Texture::add_mode_t mode, const uint8_t rotate ) -> void {
				m_texture->AddFrom( m_source_texture, mode, tc.x, tc.y, tc.x + TEXTURE_WIDTH - 1, tc.y + TEXTURE_HEIGHT - 1, tx1, ty1, rotate );
			};
			
			tile_texture_info_t txinfo;
			if ( tile->moisture == Tile::M_MOIST || tile->moisture == Tile::M_RAINY ) {
				txinfo = GetTileTextureInfo( tile, TG_MOISTURE );
			}
			else {
				txinfo.inverse_x = rand() % 2;
				txinfo.inverse_y = rand() % 2;
				txinfo.rotate_direction = rand() % 4;
				txinfo.texture_variant = 0;
			}
			
			switch ( tile->moisture ) {
				case Tile::M_NONE: {
					// invisible tile (for dev/test purposes)
					break;
				}
				case Tile::M_ARID: {
					tx_add( m_tc.arid[0], Texture::AM_COPY, txinfo.rotate_direction );
					break;
				}
				case Tile::M_MOIST: {
					txinfo = GetTileTextureInfo( tile, TG_MOISTURE );
					tx_add( m_tc.moist[ txinfo.texture_variant ], Texture::AM_COPY, txinfo.rotate_direction );
					break;
				}
				case Tile::M_RAINY: {
					txinfo = GetTileTextureInfo( tile, TG_MOISTURE );
					tx_add( m_tc.rainy[ txinfo.texture_variant ], Texture::AM_COPY, txinfo.rotate_direction );
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
					tx_add( m_tc.rocks[ rand() % 2 * 2 ], Texture::AM_MERGE, txinfo.rotate_direction );
					break;
				}
				case Tile::R_ROCKY: {
					tx_add( m_tc.rocks[ rand() % 2 * 2 + 1 ], Texture::AM_MERGE, txinfo.rotate_direction );
					break;
				}
				default:
					ASSERT( false, "invalid rockyness value" );
			}
			
			if ( tile->features	& Tile::F_JUNGLE ) {
				txinfo = GetTileTextureInfo( tile, TG_FEATURE, Tile::F_JUNGLE );
				tx_add( m_tc.jungle[ txinfo.texture_variant ], Texture::AM_MERGE, txinfo.rotate_direction );
			}
			if ( tile->features & Tile::F_XENOFUNGUS ) {
				txinfo = GetTileTextureInfo( tile, TG_FEATURE, Tile::F_XENOFUNGUS );
				tx_add( m_tc.fungus_land[ txinfo.texture_variant ], Texture::AM_MERGE, txinfo.rotate_direction );
			}
			
			if ( txinfo.inverse_x ) {
				ttx1 = tx2 - tso;
				ttx2 = tx1 + tso;
			}
			else {
				ttx1 = tx1 + tso;
				ttx2 = tx2 - tso;
			}
			if ( txinfo.inverse_y ) {
				tty1 = ty2 - tso;
				tty2 = ty1 + tso;
			}
			else {
				tty1 = ty1 + tso;
				tty2 = ty2 - tso;
			}
			
			txinfo.rotate_direction = 0; // don't rotate combined texture, everything in it is already rotated
			txvec[ ( txinfo.rotate_direction++ ) % 4 ] = { ttx1, tty2 };
			txvec[ ( txinfo.rotate_direction++ ) % 4 ] = { ttx1, tty1 };
			txvec[ ( txinfo.rotate_direction++ ) % 4 ] = { ttx2, tty1 };
			txvec[ ( txinfo.rotate_direction++ ) % 4 ] = { ttx2, tty2 };
			
			left = mesh->AddVertex( { xpos - TILE_HALFWIDTH, ypos, zc.Clamp( *tile->elevation.left ) }, { txvec[0].x * tsx, txvec[0].y * tsy } );
			top = mesh->AddVertex( { xpos, ypos - TILE_HALFHEIGHT, zc.Clamp( *tile->elevation.top ) }, { txvec[1].x * tsx, txvec[1].y * tsy } );
			right = mesh->AddVertex( { xpos + TILE_HALFWIDTH, ypos, zc.Clamp( *tile->elevation.right ) }, { txvec[2].x * tsx, txvec[2].y * tsy } );
			bottom = mesh->AddVertex( { xpos, ypos + TILE_HALFHEIGHT, zc.Clamp( tile->elevation.bottom ) }, { txvec[3].x * tsx, txvec[3].y * tsy } );
			
			center = mesh->AddVertex( { xpos, ypos, zc.Clamp( tile->elevation.center ) }, { tx * tsx, ty * tsy } );
			
			mesh->AddSurface( { center, left, top } );
			mesh->AddSurface( { center, top, right } );
			mesh->AddSurface( { center, right, bottom } );
			mesh->AddSurface( { center, bottom, left } );

		}
	}
	
	mesh->Finalize();
	
	NEW( m_terrain_actor, actor::Mesh, "MapTerrain", mesh );
		m_terrain_actor->SetTexture( m_texture );
		m_terrain_actor->SetPosition( MAP_POSITION );
		m_terrain_actor->SetAngle( MAP_ROTATION );
	m_scene->AddActor( m_terrain_actor );
}

const Map::tile_texture_info_t Map::GetTileTextureInfo( Tile* tile, const tile_grouping_criteria_t criteria, const Tile::feature_t feature ) const {
	Map::tile_texture_info_t info;
	
	info.inverse_x = 0;
	info.inverse_y = 0;
	
	Tile* source = tile;
	bool matches[ 16 ];
	uint8_t idx = 0;
	for ( uint8_t i = 0 ; i < 2 ; i++ ) {
		tile->Around( TH( this, &idx, &matches, source, criteria, feature ) {
			switch ( criteria ) {
				case TG_MOISTURE: {
					matches[ idx++ ] = tile->moisture == source->moisture;
					break;
				}
				case TG_FEATURE: {
					matches[ idx++ ] = ( tile->features & feature ) == ( source->features & feature );
					break;
				}
				default: {
					ASSERT( false, "invalid tile grouping criteria" );
				}
			}
		}, true );
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
			break;
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
