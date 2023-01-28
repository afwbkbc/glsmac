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

Map::Map( Scene* scene )
	: m_scene( scene )
{
	
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
	float tso = 1; // small offset to remove border artifacts around tiles
	
	uint8_t rotate;
	uint8_t inverse_x;
	uint8_t inverse_y;
	float ttx1, tty1, ttx2 ,tty2;
	Vec2< float > txvec[4];
	uint8_t texture_variant;
	
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
			
			/*Log(
				" X = " + to_string( x ) +
				" Y = " + to_string( y ) +
				" TX1 = " + to_string( tx1 ) +
				" TY1 = " + to_string( ty1 ) +
				" TSX1 = " + to_string( tx1 * tsx ) +
				" TSY1 = " + to_string( ty1 * tsy ) +
				" TSX2 = " + to_string( tx2 * tsx ) +
				" TSY2 = " + to_string( ty2 * tsy )
			);*/
			
			auto tx_set = [ this, tx1, ty1 ] ( const tc_t& tc ) -> void {
				m_texture->CopyFrom( m_source_texture, tc.x, tc.y, tc.x + TEXTURE_WIDTH - 1, tc.y + TEXTURE_HEIGHT - 1, tx1, ty1 );
			};
			
			inverse_x = rand() % 2;
			inverse_y = rand() % 2;
			rotate = rand() % 4;
			texture_variant = 0;
			
			if ( tile->moisture == Tile::M_MOIST || tile->moisture == Tile::M_RAINY ) {
				// pick correct texture variant and rotation depending on nearby tiles
				const bool w = tile->W->moisture == tile->moisture;
				const bool nw = tile->NW->moisture == tile->moisture;
				const bool n = tile->N->moisture == tile->moisture;
				const bool ne = tile->NE->moisture == tile->moisture;
				const bool e = tile->E->moisture == tile->moisture;
				const bool se = tile->SE->moisture == tile->moisture;
				const bool s = tile->S->moisture == tile->moisture;
				const bool sw = tile->SW->moisture == tile->moisture;
				
				const uint8_t c = w + nw + n + ne + e + se + s + sw;

				inverse_x = 0;
				inverse_y = 0;
				
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
				};
				
				auto value = tile->moisture;
				bool matches[ 16 ];
				uint8_t idx = 0;
				for ( uint8_t i = 0 ; i < 2 ; i++ ) {
					tile->Around( TH( &idx, &matches, value ) {
						matches[ idx++ ] = tile->moisture == value;
					}, true );
				}
				
				vector< uint8_t > possible_rotates = {};
				
				auto try_bitmasks = [ this, &rotate, &texture_variant, &possible_rotates, matches, bitmasks ] () {
					for ( rotate = 0 ; rotate < 8 ; rotate += 2 ) {
						uint8_t bitmask = 0;
						for ( uint8_t i = 0 ; i < 8 ; i++ ) {
							bitmask |= matches[ i + rotate ] << i;
						}
						for ( char i = bitmasks.size() - 1 ; i >= 0 ; i-- ) {
							texture_rule_t rules = bitmasks.at( i );
							if ( ( bitmask & rules.checked_bits ) == ( rules.bitmask & rules.checked_bits ) ) {
								texture_variant = i;
								possible_rotates.push_back( rotate );
								break;
							}
						}
					}
				};
				
				try_bitmasks();
				
				if ( !possible_rotates.empty() ) {
					rotate = possible_rotates[ rand() % possible_rotates.size() ] / 2; // TODO: rotate in all 8 directions?
				}
				else {
					texture_variant = 16;
				}
				
			}
			
			switch ( tile->moisture ) {
				case Tile::M_NONE: {
					// invisible tile (for dev/test purposes)
					break;
				}
				case Tile::M_ARID: {
					tx_set( m_tc.arid[0] );
					break;
				}
				case Tile::M_MOIST: {
					if ( texture_variant != 16 ) // tmp
						tx_set( m_tc.moist[ texture_variant ] );
					break;
				}
				case Tile::M_RAINY: {
					if ( texture_variant != 16 ) // tmp
						tx_set( m_tc.rainy[ texture_variant ] );
					break;
				}
				default:
					ASSERT( false, "invalid moisture value" );
			}
			
			
			if ( inverse_x ) {
				ttx1 = tx2 - tso;
				ttx2 = tx1 + tso;
			}
			else {
				ttx1 = tx1 + tso;
				ttx2 = tx2 - tso;
			}
			if ( inverse_y ) {
				tty1 = ty2 - tso;
				tty2 = ty1 + tso;
			}
			else {
				tty1 = ty1 + tso;
				tty2 = ty2 - tso;
			}
			
			txvec[ ( rotate++ ) % 4 ] = { ttx1, tty2 };
			txvec[ ( rotate++ ) % 4 ] = { ttx1, tty1 };
			txvec[ ( rotate++ ) % 4 ] = { ttx2, tty1 };
			txvec[ ( rotate++ ) % 4 ] = { ttx2, tty2 };
			
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

}
}
}
