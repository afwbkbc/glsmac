#include "Map.h"

#include "engine/Engine.h"

#include "types/Mesh.h"

namespace game {
namespace world {
namespace map {

// for development/debug only
#define WIREFRAMES 0
#define WIREFRAMES_GRADIENTS 0

#define MAP_POSITION { 0.0, 0.0, 0.0 }
#define MAP_ROTATION { 0.0, 0.0, 0.0 }

#define TILE_WIDTH 1.0f
#define TILE_HEIGHT 1.0f
#define TILE_Z_SCALE 2.0f

// must match PCX
#define TEXTURE_WIDTH 56
#define TEXTURE_HEIGHT 56

#define TILE_HALFWIDTH ( TILE_WIDTH / 2 )
#define TILE_HALFHEIGHT ( TILE_HEIGHT / 2 )
#define TEXTURE_HALFWIDTH ( TEXTURE_WIDTH / 2 )
#define TEXTURE_HALFHEIGHT ( TEXTURE_HEIGHT / 2 )

#if WIREFRAMES
static Texture* s_wireframe_texture = nullptr;
#endif

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
	
#if WIREFRAMES
	NEW( s_wireframe_texture, Texture, "MapWireframe", TEXTURE_WIDTH, TEXTURE_HEIGHT );
#if WIREFRAMES_GRADIENTS
	float mx = 1.0f / TEXTURE_WIDTH;
	float my = 1.0f / TEXTURE_HEIGHT;
	for ( size_t y = 0 ; y < TEXTURE_HEIGHT ; y++ ) {
		for ( size_t x = 0 ; x < TEXTURE_WIDTH ; x++ ) {
			s_wireframe_texture->SetPixel( x, y, { mx * x / 2, my * y / 2, 0.0f, 1.0f } );
		}
	}
#else
	Color color_h = { 0.0f, 1.0f, 0.0f, 1.0f };
	Color color_v = { 1.0f, 0.0f, 0.0f, 1.0f };
	Color color_hv = { 1.0f, 1.0f, 0.0f, 1.0f };
	Color value;
	bool value_x = true;
	bool value_y = true;
	for ( size_t y = 0 ; y < TEXTURE_HEIGHT ; y++ ) {
		for ( size_t x = 0 ; x < TEXTURE_WIDTH ; x++ ) {
			if ( value_y ) {
				if ( value_x ) {
					value = color_hv;
				}
				else {
					value = color_h;
				}
			}
			else if ( value_x ) {
				value = color_v;
			}
			s_wireframe_texture->SetPixel( x, y, value );
			value_x = !value_x;
		}
		value_y = !value_y;
	}
#endif
#endif
	
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
#if WIREFRAMES
	DELETE( s_wireframe_texture );
	s_wireframe_texture = nullptr;
#endif
	
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

	typedef struct {
		types::Mesh::index_t center;
		types::Mesh::index_t left;
		types::Mesh::index_t right;
		types::Mesh::index_t top;
		types::Mesh::index_t bottom;
	} indices_t;
	indices_t v_land, v_water, v_water_surface;
	
	NEW( m_textures.terrain, Texture, "TerrainTexture", w * TEXTURE_WIDTH, ( h * o_max ) * TEXTURE_HEIGHT );
	NEWV( mesh_terrain, types::Mesh, w * ( h * o_max ) * 5 / 2, w * ( h * o_max ) * 4 / 2 );
	
	float xpos, ypos;
	float tx, ty, tx1, ty1, tx2, ty2;
	
	int8_t em;
	
	float ox = -( (float) TILE_WIDTH * m_tiles->GetWidth() / 4 - TILE_HALFWIDTH );
	float oy = -( (float) TILE_HEIGHT * m_tiles->GetHeight() / 4 - TILE_HALFHEIGHT );
	
	util::Clamper< float > elevation_to_vertex_z( Tile::ELEVATION_MIN, Tile::ELEVATION_MAX, -TILE_Z_SCALE, TILE_Z_SCALE );
	util::Clamper< float > elevation_to_water_r( Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.6f, 1.3f );
	util::Clamper< float > elevation_to_water_g( Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.6f, 1.8f );
	util::Clamper< float > elevation_to_water_b( Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 0.8f, 1.8f ); // sea bottom is very blue so need less here
	util::Clamper< float > elevation_to_water_a( Tile::ELEVATION_LEVEL_TRENCH, Tile::ELEVATION_LEVEL_COAST, 1.0f, 0.5f );
	
	//Color coastline_tint = { 1.0f, 1.0f, 1.0f, 1.0f };
	Color coastline_tint = { 0.6f, 0.7f, 1.0f, 1.0f };
	Color coastline_tint_land;
	
	Tile* tile;
	
	float tsx = (float) 1 / TEXTURE_WIDTH / w;
	float tsy = (float) 1 / TEXTURE_HEIGHT / h / o_max;
	
	struct {
		Vec3 center;
		Vec3 left;
		Vec3 top;
		Vec3 right;
		Vec3 bottom;
	} vc = {};
	
	// small padding to reduce border artifacts (they look like tile border) around tiles // <- why does it happen?
	float tsp = 0.0f;
	
	float ttx1, tty1, ttx2 ,tty2;
	struct {
		Vec2< float > center;
		Vec2< float > left;
		Vec2< float > top;
		Vec2< float > right;
		Vec2< float > bottom;
	} txvec, txvec_land;

	struct {
		Color center;
		Color left;
		Color top;
		Color right;
		Color bottom;
	} tint_land, tint_water;
	
	vector< pair< types::Mesh::index_t, types::Mesh::index_t > > copy_normals = {};
	unordered_map< size_t, indices_t > saved_indices = {};
	unordered_map< types::Mesh::index_t, pair< size_t, Texture::add_mode_t > > need_normals = {};
	
	typedef struct {
		Texture::add_mode_t mode;
		size_t tx1_from;
		size_t ty1_from;
		size_t tx2_from;
		size_t ty2_from;
		size_t tx_to;
		size_t ty_to;
		uint8_t rotate;
		float alpha;
	} copy_from_after_t;
	vector< copy_from_after_t > copy_from_after;
	
	tile_texture_info_t txinfo;
	bool has_water;
	bool is_coastline;
	
	// coastline stuff
	const Texture::add_mode_t coastline_mode = Texture::AM_MERGE | Texture::AM_INVERT;
	typedef struct {
		size_t msx;
		size_t msy;
		Texture::add_mode_t side;
		bool can_mirror;
		bool maybe_mirror_nw;
		bool maybe_mirror_ne;
		bool maybe_mirror_se;
		bool maybe_mirror_sw;
		Texture::add_mode_t mirror_mode;
	} coastline_corner_t;
	vector< coastline_corner_t > coastline_corners;
	coastline_corner_t coastline_corner_tmp;
	
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
#if WIREFRAMES
				m_textures.terrain->AddFrom( s_wireframe_texture, mode, 0, 0, TEXTURE_WIDTH - 1, TEXTURE_HEIGHT - 1, tx1, offset * h * TEXTURE_HEIGHT + ty1, rotate, alpha );
#else
				m_textures.terrain->AddFrom( m_textures.source, mode, tc.x, tc.y, tc.x + TEXTURE_WIDTH - 1, tc.y + TEXTURE_HEIGHT - 1, tx1, offset * h * TEXTURE_HEIGHT + ty1, rotate, alpha );
#endif
			};
			
			auto tx_copy_from = [ this, tx1, ty1, h ] ( const size_t offset_from, const size_t tx_from, const size_t ty_from,const size_t offset, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha = 1.0f ) -> void {
				m_textures.terrain->AddFrom( m_textures.terrain, mode, tx_from, offset_from * h * TEXTURE_HEIGHT + ty_from, tx_from + TEXTURE_WIDTH - 1, offset_from * h * TEXTURE_HEIGHT + ty_from + TEXTURE_HEIGHT - 1, tx1, offset * h * TEXTURE_HEIGHT + ty1, rotate, alpha );
			};
			auto tx_copy = [ tx_copy_from, tx1, ty1 ] ( const size_t offset_from, const size_t offset, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha = 1.0f ) -> void {
				tx_copy_from( offset_from, tx1, ty1, offset, mode, rotate, alpha );
			};
			
			// copy after everything was generated
			auto tx_copy_from_after = [ this, tx1, ty1, h, &copy_from_after ] ( const size_t offset_from, const size_t tx_from, const size_t ty_from,const size_t offset, const Texture::add_mode_t mode, const uint8_t rotate, const float alpha = 1.0f ) -> void {
				copy_from_after.push_back({ mode, tx_from, offset_from * h * TEXTURE_HEIGHT + ty_from, tx_from + TEXTURE_WIDTH - 1, offset_from * h * TEXTURE_HEIGHT + ty_from + TEXTURE_HEIGHT - 1, (size_t)tx1, offset * h * TEXTURE_HEIGHT + (size_t)ty1, rotate, alpha });
			};

			txinfo.inverse_x = 0;
			txinfo.inverse_y = 0;
			txinfo.rotate_direction = 0;
			txinfo.texture_variant = 0;
			
			Tile::elevation_t e_left = *tile->elevation.left;
			Tile::elevation_t e_top = *tile->elevation.top;
			Tile::elevation_t e_right = *tile->elevation.right;
			Tile::elevation_t e_bottom = *tile->elevation.bottom;
			Tile::elevation_t e_center = *tile->elevation.center;
			
			switch ( tile->moisture ) {
				case Tile::M_NONE: {
					// invisible tile (for dev/test purposes)
					break;
				}
				case Tile::M_ARID: {
					tx_add( o_land, m_tc.arid[0], Texture::AM_DEFAULT, txinfo.rotate_direction );
					break;
				}
				case Tile::M_MOIST: {
					txinfo = GetTileTextureInfo( tile, TG_MOISTURE );
					tx_add( o_land, m_tc.moist[ txinfo.texture_variant ], Texture::AM_DEFAULT, txinfo.rotate_direction );
					break;
				}
				case Tile::M_RAINY: {
					txinfo = GetTileTextureInfo( tile, TG_MOISTURE );
					tx_add( o_land, m_tc.rainy[ txinfo.texture_variant ], Texture::AM_DEFAULT, txinfo.rotate_direction );
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

			if ( tile->features	& Tile::F_JUNGLE ) {
				txinfo = GetTileTextureInfo( tile, TG_FEATURE, Tile::F_JUNGLE );
				tx_add( o_land, m_tc.jungle[ txinfo.texture_variant ], Texture::AM_MERGE, txinfo.rotate_direction );
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
			
			txvec.center = { tx, ty };
			txvec.left = { ttx1, tty2 };
			txvec.top = { ttx1, tty1 };
			txvec.right = { ttx2, tty1 };
			txvec.bottom = { ttx2, tty2 };

			txvec_land = txvec; // need separate because it can be modified for coastlines
			
			// modify elevations based on water / not water, to avoid displaying half-submerged tiles
			// original tile isn't modified, this is just for rendering
			em = tile->is_water_tile ? -1 : 3; // setting -1 : 100 gives interesting shadow effect, but it's not very realistic
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
			
			// vertex coodinates for land corners
			#define vcz( _val ) elevation_to_vertex_z.Clamp( _val )
			vc.left = { xpos - TILE_HALFWIDTH, ypos, vcz( e_left ) };
			vc.top = { xpos, ypos - TILE_HALFHEIGHT, vcz( e_top ) };
			vc.right = { xpos + TILE_HALFWIDTH, ypos, vcz( e_right ) };
			vc.bottom = { xpos, ypos + TILE_HALFHEIGHT, vcz( e_bottom ) };
			vc.center = { ( vc.left.x + vc.right.x ) / 2, ( vc.top.y + vc.bottom.y ) / 2, vcz( e_center ) };
			
			tint_land = tint_water = {
				(Color){ 1.0, 1.0, 1.0, 1.0 },
				(Color){ 1.0, 1.0, 1.0, 1.0 },
				(Color){ 1.0, 1.0, 1.0, 1.0 },
				(Color){ 1.0, 1.0, 1.0, 1.0 },
				(Color){ 1.0, 1.0, 1.0, 1.0 },
			};
			
			is_coastline = false;
			
			// it is what it is
			float cw = 0.06f;
			float cwc = 1.5f;
			float tcw = cw * 2.0f;
			float tcwc = cwc * 0.85f;
			float tcww = tcw * TEXTURE_WIDTH;
			float tcwh = tcw * TEXTURE_HEIGHT;
			coastline_tint_land = coastline_tint; // * ( 0.95f ); // ?
			if ( !tile->is_water_tile ) {
				if ( tile->W->is_water_tile || tile->NW->is_water_tile || tile->SW->is_water_tile ) {
					tint_land.left = coastline_tint_land;
					if ( tile->W->is_water_tile && ( tile->NW->is_water_tile || tile->SW->is_water_tile ) ) {
						vc.left.x += cw;
						if ( !tile->NW->is_water_tile ) {
							vc.left.y -= cw;
							txvec_land.left.y -= tcwh;
						}
						else if ( !tile->SW->is_water_tile ) {
							vc.left.y += cw;
							txvec_land.left.x += tcww;
						}
						else {
							vc.left.x += cw * cwc;
							txvec_land.left.y -= tcwh * tcwc;
							txvec_land.left.x += tcww * tcwc;
						}
						is_coastline = true;
					}
				}
				if ( tile->N->is_water_tile || tile->NW->is_water_tile || tile->NE->is_water_tile ) {
					tint_land.top = coastline_tint_land;
					if ( tile->N->is_water_tile && ( tile->NW->is_water_tile || tile->NE->is_water_tile ) ) {
						vc.top.y += cw;
						if ( !tile->NW->is_water_tile ) {
							vc.top.x -= cw;
							txvec_land.top.y += tcwh;
						}
						else if ( !tile->NE->is_water_tile ) {
							vc.top.x += cw;
							txvec_land.top.x += tcww;
						}
						else {
							vc.top.y += cw * cwc;
							txvec_land.top.x += tcww * tcwc;
							txvec_land.top.y += tcwh * tcwc;
						}
						is_coastline = true;
					}
				}
				if ( tile->E->is_water_tile || tile->NE->is_water_tile || tile->SE->is_water_tile ) {
					tint_land.right = coastline_tint_land;
					if ( tile->E->is_water_tile && ( tile->NE->is_water_tile || tile->SE->is_water_tile ) ) {
						vc.right.x -= cw;
						if ( !tile->NE->is_water_tile ) {
							vc.right.y -= cw;
							txvec_land.right.x -= tcww;
						}
						else if ( !tile->SE->is_water_tile ) {
							vc.right.y += cw;
							txvec_land.right.y += tcwh;
						}
						else {
							vc.right.x -= cw * cwc;
							txvec_land.right.x -= tcww * tcwc;
							txvec_land.right.y += tcwh * tcwc;
						}
						is_coastline = true;
					}
				}
				if ( tile->S->is_water_tile || tile->SW->is_water_tile || tile->SE->is_water_tile ) {
					tint_land.bottom = coastline_tint_land;
					if ( tile->S->is_water_tile && ( tile->SW->is_water_tile || tile->SE->is_water_tile ) ) {
						vc.bottom.y -= cw;
						if ( !tile->SW->is_water_tile ) {
							vc.bottom.x -= cw;
							txvec_land.bottom.x -= tcww;
						}
						else if ( !tile->SE->is_water_tile ) {
							vc.bottom.x += cw;
							txvec_land.bottom.y -= tcwh;
						}
						else {
							vc.bottom.y -= cw * cwc;
							txvec_land.bottom.x -= tcww * tcwc;
							txvec_land.bottom.y -= tcwh * tcwc;
						}
						is_coastline = true;
					}
				}
				txvec_land.center.x = ( txvec_land.left.x + txvec_land.top.x + txvec_land.right.x + txvec_land.bottom.x ) / 4;
				txvec_land.center.y = ( txvec_land.left.y + txvec_land.top.y + txvec_land.right.y + txvec_land.bottom.y ) / 4;
			}
			
			// average new center from computed corners
			e_center = ( e_left + e_top + e_right + e_bottom ) / 4;
			vc.center = { ( vc.left.x + vc.right.x ) / 2, ( vc.top.y + vc.bottom.y ) / 2, vcz( e_center ) };

			has_water = (
				is_coastline ||
				e_center <= Tile::ELEVATION_LEVEL_COAST ||
				e_left <= Tile::ELEVATION_LEVEL_COAST ||
				e_top <= Tile::ELEVATION_LEVEL_COAST ||
				e_right <= Tile::ELEVATION_LEVEL_COAST ||
				e_bottom <= Tile::ELEVATION_LEVEL_COAST
			);
			
			if ( has_water ) {
				
				tx_add( o_water_surface, m_tc.water[ 1 ], Texture::AM_DEFAULT, rand() % 4 );
				
				// corners on land tiles
				if ( is_coastline ) {
					
					Texture::add_mode_t mode = Texture::AM_MERGE;
					if ( tile->W->is_water_tile && tile->SW->is_water_tile && tile->NW->is_water_tile ) {
						mode |= Texture::AM_ROUND_LEFT;
					}
					if ( tile->N->is_water_tile && tile->NW->is_water_tile && tile->NE->is_water_tile ) {
						mode |= Texture::AM_ROUND_TOP;
					}
					if ( tile->E->is_water_tile && tile->SE->is_water_tile && tile->NE->is_water_tile ) {
						mode |= Texture::AM_ROUND_RIGHT;
					}
					if ( tile->S->is_water_tile && tile->SE->is_water_tile && tile->SW->is_water_tile ) {
						mode |= Texture::AM_ROUND_BOTTOM;
					}
					
					// coastline tint
					if ( tile->W->is_water_tile || tile->SW->is_water_tile || tile->NW->is_water_tile ) {
						tint_water.left = coastline_tint;
					}
					if ( tile->N->is_water_tile || tile->NW->is_water_tile || tile->NE->is_water_tile ) {
						tint_water.top = coastline_tint;
					}
					if ( tile->E->is_water_tile || tile->SE->is_water_tile || tile->NE->is_water_tile ) {
						tint_water.right = coastline_tint;
					}
					if ( tile->S->is_water_tile || tile->SW->is_water_tile || tile->SE->is_water_tile ) {
						tint_water.bottom = coastline_tint;
					}

					// copy texture from land
					tx_copy( o_land, o_water, mode, 0 );
					
				}
				
				// corners on water tiles
				if ( tile->is_water_tile ) {
					
					coastline_corners.clear();
					
					if ( !tile->SW->is_water_tile && !tile->NW->is_water_tile ) {
						coastline_corner_tmp.side = Texture::AM_ROUND_LEFT;
						tint_water.left = coastline_tint;
						if ( !tile->W->is_water_tile ) {
							coastline_corner_tmp.can_mirror = true;
							if ( x >= 2 ) {
								coastline_corner_tmp.msx = x - 2;
							}
							else {
								coastline_corner_tmp.msx = w - 2;
							}
							coastline_corner_tmp.msy = y;
						}
						else {
							coastline_corner_tmp.maybe_mirror_sw = true;
							coastline_corner_tmp.maybe_mirror_nw = true;
						}
						coastline_corners.push_back( coastline_corner_tmp );
					}
					
					if ( !tile->NW->is_water_tile && !tile->NE->is_water_tile ) {
						coastline_corner_tmp.side = Texture::AM_ROUND_TOP;
						tint_water.top = coastline_tint;
						if ( !tile->N->is_water_tile ) {
							if ( y >= 2 ) {
								coastline_corner_tmp.can_mirror = true;
								coastline_corner_tmp.msx = x;
								coastline_corner_tmp.msy = y - 2;
							}
						}
						else {
							coastline_corner_tmp.maybe_mirror_nw = true;
							coastline_corner_tmp.maybe_mirror_ne = true;
						}
						coastline_corners.push_back( coastline_corner_tmp );
					}
					if ( !tile->SE->is_water_tile && !tile->NE->is_water_tile ) {
						coastline_corner_tmp.side = Texture::AM_ROUND_RIGHT;
						tint_water.right = coastline_tint;
						if ( !tile->E->is_water_tile ) {
							coastline_corner_tmp.can_mirror = true;
							if ( x < w - 2 ) {
								coastline_corner_tmp.msx = x + 2;
							}
							else {
								coastline_corner_tmp.msx = 0;
							}
							coastline_corner_tmp.msy = y;
						}
						else {
							coastline_corner_tmp.maybe_mirror_se = true;
							coastline_corner_tmp.maybe_mirror_ne = true;
						}
						coastline_corners.push_back( coastline_corner_tmp );
					}
					if ( !tile->SE->is_water_tile && !tile->SW->is_water_tile ) {
						coastline_corner_tmp.side = Texture::AM_ROUND_BOTTOM;
						tint_water.bottom = coastline_tint;
						if ( !tile->S->is_water_tile ) {
							if ( y < h - 2 ) {
								coastline_corner_tmp.can_mirror = true;
								coastline_corner_tmp.msx = x;
								coastline_corner_tmp.msy = y + 2;
							}
						}
						else {
							coastline_corner_tmp.maybe_mirror_se = true;
							coastline_corner_tmp.maybe_mirror_sw = true;
						}
						coastline_corners.push_back( coastline_corner_tmp );
					}
					
					for ( auto& c : coastline_corners ) {
						
						if ( c.can_mirror ) {
							c.mirror_mode = Texture::AM_MIRROR_X | Texture::AM_MIRROR_Y;
						}
						
						if ( !c.can_mirror && y >= 1 ) {
							c.msy = y - 1;
							if ( !c.can_mirror && coastline_corner_tmp.maybe_mirror_nw ) {
								if ( x >= 1 ) {
									c.msx = x - 1;
								}
								else {
									c.msx = w - 2;
								}
								c.mirror_mode = Texture::AM_MIRROR_X;
								c.can_mirror = true;
							}
							if ( !c.can_mirror && coastline_corner_tmp.maybe_mirror_ne ) {
								if ( x < w - 1 ) {
									c.msx = x + 1;
								}
								else {
									c.msx = 1;
								}
								c.mirror_mode = Texture::AM_MIRROR_Y;
								c.can_mirror = true;
							}
						}
						if ( !c.can_mirror && y < h - 1 ) {
							c.msy = y + 1;
							if ( !c.can_mirror && coastline_corner_tmp.maybe_mirror_sw ) {
								if ( x >= 1 ) {
									c.msx = x - 1;
								}
								else {
									c.msx = w - 1;
								}
								c.mirror_mode = Texture::AM_MIRROR_X;
								c.can_mirror = true;
							}
							if ( !c.can_mirror && coastline_corner_tmp.maybe_mirror_se ) {
								if ( x < w - 1 ) {
									c.msx = x + 1;
								}
								else {
									c.msx = 1;
								}
								c.mirror_mode = Texture::AM_MIRROR_Y;
								c.can_mirror = true;
							}
						}
						
						if ( c.can_mirror ) {
							// mirror opposite tile
							tx_copy_from_after( o_land, c.msx * TEXTURE_WIDTH, c.msy * TEXTURE_HEIGHT, o_water, coastline_mode | c.side | c.mirror_mode, 0 );
						}
						else {
							// use default texture
							tx_add( o_water, m_tc.arid[ 0 ], coastline_mode | c.side, rand() % 4 );
						}
					}
				}
			}
			
			// land
			{
				// texture coordinates (land)
				#define txc( _o, _c ) { txvec_land._c.x * tsx, ( txvec_land._c.y + _o * h * TEXTURE_HEIGHT ) * tsy }
				// tint (depending on if this tile is underwater or not)
				#define txt( _val ) ( tile->is_water_tile ? (Color){ 0.0, 0.2, 0.5, 1.0 } : tint_land._val )
				
				v_land.center = mesh_terrain->AddVertex( vc.center, txc( o_land, center ), txt( center ) );
				v_land.left = mesh_terrain->AddVertex( vc.left, txc( o_land, left ), txt( left ) );
				v_land.top = mesh_terrain->AddVertex( vc.top, txc( o_land, top ), txt( top ) );
				v_land.right = mesh_terrain->AddVertex( vc.right, txc( o_land, right ), txt( right ) );
				v_land.bottom = mesh_terrain->AddVertex( vc.bottom, txc( o_land, bottom ), txt( bottom ) );
				mesh_terrain->AddSurface( { v_land.center, v_land.left, v_land.top } );
				mesh_terrain->AddSurface( { v_land.center, v_land.top, v_land.right } );
				mesh_terrain->AddSurface( { v_land.center, v_land.right, v_land.bottom } );
				mesh_terrain->AddSurface( { v_land.center, v_land.bottom, v_land.left } );
				
				// if applicable - save normals to be used for water tile corners later
				if ( !tile->is_water_tile &&
					( tile->W->is_water_tile && !tile->NW->is_water_tile && !tile->SW->is_water_tile ) ||
					( tile->N->is_water_tile && !tile->NW->is_water_tile && !tile->NE->is_water_tile ) ||
					( tile->E->is_water_tile && !tile->NE->is_water_tile && !tile->SE->is_water_tile ) ||
					( tile->S->is_water_tile && !tile->SE->is_water_tile && !tile->SW->is_water_tile )
				) {
					//Log( "Saving normals of " + to_string( y * w + x ) );
					saved_indices[ y * w + x ] = v_land;
				}
				
				#undef txt
				#undef txc
			}
			
			// texture coordinates
			#define txc( _o, _c ) { txvec._c.x * tsx, ( txvec._c.y + _o * h * TEXTURE_HEIGHT ) * tsy }
			
			float water_z = elevation_to_vertex_z.Clamp( Tile::ELEVATION_LEVEL_COAST ); // sea is always on sea level
			
			// water surface
			{
				#define wr elevation_to_water_r.Clamp
				#define wg elevation_to_water_g.Clamp
				#define wb elevation_to_water_b.Clamp
				#define wa elevation_to_water_a.Clamp
				#define txt( _val ) { wr( _val ), wg( _val ), wb( _val ), wa( _val ) }  // tint

				v_water_surface.center = mesh_terrain->AddVertex( { xpos, ypos, water_z }, txc( o_water_surface, center ), txt( e_center) );
				v_water_surface.left = mesh_terrain->AddVertex( { xpos - TILE_HALFWIDTH, ypos, water_z }, txc( o_water_surface, left ), txt( e_left ) );
				v_water_surface.top = mesh_terrain->AddVertex( { xpos, ypos - TILE_HALFHEIGHT, water_z }, txc( o_water_surface, top ), txt( e_top ) );
				v_water_surface.right = mesh_terrain->AddVertex( { xpos + TILE_HALFWIDTH, ypos, water_z }, txc( o_water_surface, right ), txt( e_right ) );
				v_water_surface.bottom = mesh_terrain->AddVertex( { xpos, ypos + TILE_HALFHEIGHT, water_z }, txc( o_water_surface, bottom ), txt( e_bottom ) );
				mesh_terrain->AddSurface( { v_water_surface.center, v_water_surface.left, v_water_surface.top } );
				mesh_terrain->AddSurface( { v_water_surface.center, v_water_surface.top, v_water_surface.right } );
				mesh_terrain->AddSurface( { v_water_surface.center, v_water_surface.right, v_water_surface.bottom } );
				mesh_terrain->AddSurface( { v_water_surface.center, v_water_surface.bottom, v_water_surface.left } );
				
				#undef txt
				#undef wr
				#undef wg
				#undef wb
				#undef wa
			}
			
			// water
			{
				#define txt( _val ) ( tint_water._val )
				
				v_water.center = mesh_terrain->AddVertex( { xpos, ypos, water_z }, txc( o_water, center ), txt( center ) );
				v_water.left = mesh_terrain->AddVertex( { xpos - TILE_HALFWIDTH, ypos, water_z }, txc( o_water, left ), txt( left ) );
				v_water.top = mesh_terrain->AddVertex( { xpos, ypos - TILE_HALFHEIGHT, water_z }, txc( o_water, top ), txt( top ) );
				v_water.right = mesh_terrain->AddVertex( { xpos + TILE_HALFWIDTH, ypos, water_z }, txc( o_water, right ), txt( right ) );
				v_water.bottom = mesh_terrain->AddVertex( { xpos, ypos + TILE_HALFHEIGHT, water_z }, txc( o_water, bottom ), txt( bottom ) );
				mesh_terrain->AddSurface( { v_water.center, v_water.left, v_water.top } );
				mesh_terrain->AddSurface( { v_water.center, v_water.top, v_water.right } );
				mesh_terrain->AddSurface( { v_water.center, v_water.right, v_water.bottom } );
				mesh_terrain->AddSurface( { v_water.center, v_water.bottom, v_water.left } );
				
				if ( is_coastline ) {
					// fix incorrect shadows on coasts (because land vertices were moved)
					// TODO: set only needed ones?
					copy_normals.push_back( { v_land.left, v_water.left } );
					copy_normals.push_back( { v_land.top, v_water.top } );
					copy_normals.push_back( { v_land.right, v_water.right } );
					copy_normals.push_back( { v_land.bottom, v_water.bottom } );
					copy_normals.push_back( { v_land.center, v_water.center } );
				}
				
				if ( tile->is_water_tile ) {
					// get normal from corresponding adjactent tile
					for ( auto& c : coastline_corners ) {
						//Log( "Need normals of " + to_string( c.msy * w + c.msx ) );
						need_normals[
							c.side == Texture::AM_ROUND_LEFT ? v_water.left :
							c.side == Texture::AM_ROUND_TOP ? v_water.top :
							c.side == Texture::AM_ROUND_RIGHT ? v_water.right :
							c.side == Texture::AM_ROUND_BOTTOM ? v_water.bottom : -1
						] = { c.msy * w + c.msx, c.side };
					}
				}
				
				#undef txt
			}
		}
	}
	
	for ( auto& c : copy_from_after ) {
		m_textures.terrain->AddFrom( m_textures.terrain, c.mode, c.tx1_from, c.ty1_from, c.tx2_from, c.ty2_from, c.tx_to, c.ty_to, c.rotate, c.alpha );
	}
	
	mesh_terrain->Finalize();
	
	Log( "Updating normals" );
	// update normals where needed
	for ( auto& cn : copy_normals ) {
		mesh_terrain->SetVertexNormal( cn.second, mesh_terrain->GetVertexNormal( cn.first ) );
	}
	for ( auto& nn : need_normals ) {
		auto it = saved_indices.find( nn.second.first );
		if ( it != saved_indices.end() ) {
			mesh_terrain->SetVertexNormal( nn.first, mesh_terrain->GetVertexNormal(
				nn.second.second == Texture::AM_ROUND_LEFT ? it->second.right :
				nn.second.second == Texture::AM_ROUND_TOP ? it->second.bottom :
				nn.second.second == Texture::AM_ROUND_RIGHT ? it->second.left :
				nn.second.second == Texture::AM_ROUND_BOTTOM ? it->second.top : -1
			));
		}
	}
	
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
						( t->is_water_tile == tile->is_water_tile ) &&
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
		info.rotate_direction = possible_rotates[ rand() % possible_rotates.size() ] / 2;
	}
	else {
		ASSERT( false, "could not find texture variant" );
	}
	
	return info;
}

}
}
}
