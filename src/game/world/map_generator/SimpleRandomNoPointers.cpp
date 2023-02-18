#include "SimpleRandomNoPointers.h"

#ifdef MAPGEN_BENCHMARK

// to check "pointers are slow" theory

namespace game {
namespace world {
namespace map_generator {

void SimpleRandomNoPointers::Generate( Tiles* tiles, size_t seed ) {
	Log( "Generating terrain ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );
	
	const size_t width = tiles->GetWidth();
	const size_t height = tiles->GetHeight();
	
	Tile* tile;
	for ( auto y = 0 ; y < height ; y++ ) {
		for ( auto x = y & 1 ; x < width ; x += 2 ) {
			tile = tiles->At( x, y );
		
#define RND MAPGEN_ELEVATION_MIN + tiles->GetRandom()->GetUInt( 0, MAPGEN_ELEVATION_MAX - MAPGEN_ELEVATION_MIN )
			
			if ( tile->coord.y > 1 ) {
				tiles->At( x, y - 2 )->elevation_data.bottom = RND;
			}
			if ( tile->coord.y > 0 ) {
				tiles->At( x > 0 ? x - 1 : width - 1, y - 1 )->elevation_data.bottom = RND;
				tiles->At( x < width - 1 ? x + 1 : 0, y - 1 )->elevation_data.bottom = RND;
			}
			tile->elevation_data.bottom = RND;
			
			// random moisture etc
			tile->moisture = tiles->GetRandom()->GetUInt( 1, 3 );
			tile->rockyness = tiles->GetRandom()->GetUInt( 1, 3 );
			tile->features = 0;
			
			// spawn some jungles
			if ( tiles->GetRandom()->IsLucky( 30 ) ) {
				tile->features |= Tile::F_JUNGLE;

				#define JUNGLES2( _x, _y ) { \
					if ( tiles->GetRandom()->IsLucky( 2 ) ) { \
						tiles->At( (_x), (_y) )->features |= Tile::F_JUNGLE; \
					} \
				}
				
				#define JUNGLES1( _x, _y ) { \
					if ( tiles->GetRandom()->IsLucky( 2 ) ) { \
						tiles->At( (_x), (_y) )->features |= Tile::F_JUNGLE; \
						if ( (_y) > 1 ) { \
							JUNGLES2( (_x), (_y) - 2 ); \
						} \
						if ( (_y) > 0 ) { \
							JUNGLES2( (_x) > 0 ? (_x) - 1 : width - 1, (_y) - 1 ); \
							JUNGLES2( (_x) < width - 1 ? (_x) + 1 : 0, (_y) - 1 ); \
						} \
						JUNGLES2( (_x) > 1 ? (_x) - 2 : width - 1 - ( 1 - ( (_y) % 2 ) ), (_y) ); \
						JUNGLES2( (_x) < width - 2 ? (_x) + 2 : (_y) % 2, (_y) ); \
						if ( (_y) > 1 ) { \
							JUNGLES2( (_x), (_y) - 2 ); \
						} \
						if ( (_y) < height - 1 ) { \
							JUNGLES2( (_x) > 0 ? (_x) - 1 : width - 1, (_y) + 1 ); \
							JUNGLES2( (_x) < width - 1 ? (_x) + 1 : 0, (_y) + 1 ); \
						} \
						if ( (_y) < height - 2 ) { \
							JUNGLES2( (_x), (_y) + 2 ); \
						} \
					} \
				}

				if ( y > 1 ) {
					JUNGLES1( x, y - 2 ); // N
				}
				if ( y > 0 ) {
					JUNGLES1( x > 0 ? x - 1 : width - 1, y - 1 ); // NW
					JUNGLES1( x < width - 1 ? x + 1 : 0, y - 1 ); // NE
				}
				JUNGLES1( x > 1 ? x - 2 : width - 1 - ( 1 - ( y % 2 ) ), y ); // W
				JUNGLES1( x < width - 2 ? x + 2 : y % 2, y ); // E
				if ( y > 1 ) {
					JUNGLES1( x, y - 2 ); // N
				}
				if ( y < height - 1 ) {
					JUNGLES1( x > 0 ? x - 1 : width - 1, y + 1 ); // SW
					JUNGLES1( x < width - 1 ? x + 1 : 0, y + 1 ); // SE
				}
				if ( y < height - 2 ) {
					JUNGLES1( x, y + 2 ); // S
				}
				#undef JUNGLES1
				#undef JUNGLES2
			}
			
			if ( tiles->GetRandom()->IsLucky( 40 ) ) {
				
				tile->features |= Tile::F_XENOFUNGUS;
				
				#define FUNGUS4( _x, _y ) { \
					if ( tiles->GetRandom()->IsLucky( 5 ) ) { \
						tiles->At( (_x), (_y) )->features |= Tile::F_XENOFUNGUS; \
					} \
				}
				
				#define FUNGUS3( _x, _y ) { \
					if ( tiles->GetRandom()->IsLucky( 4 ) ) { \
						tiles->At( (_x), (_y) )->features |= Tile::F_XENOFUNGUS; \
						if ( (_y) > 1 ) { \
							FUNGUS4( (_x), (_y) - 2 ); \
						} \
						if ( (_y) > 0 ) { \
							FUNGUS4( (_x) > 0 ? (_x) - 1 : width - 1, (_y) - 1 ); \
							FUNGUS4( (_x) < width - 1 ? (_x) + 1 : 0, (_y) - 1 ); \
						} \
						FUNGUS4( (_x) > 1 ? (_x) - 2 : width - 1 - ( 1 - ( (_y) % 2 ) ), (_y) ); \
						FUNGUS4( (_x) < width - 2 ? (_x) + 2 : (_y) % 2, (_y) ); \
						if ( (_y) > 1 ) { \
							FUNGUS4( (_x), (_y) - 2 ); \
						} \
						if ( (_y) < height - 1 ) { \
							FUNGUS4( (_x) > 0 ? (_x) - 1 : width - 1, (_y) + 1 ); \
							FUNGUS4( (_x) < width - 1 ? (_x) + 1 : 0, (_y) + 1 ); \
						} \
						if ( (_y) < height - 2 ) { \
							FUNGUS4( (_x), (_y) + 2 ); \
						} \
					} \
				}

				#define FUNGUS2( _x, _y ) { \
					if ( tiles->GetRandom()->IsLucky( 3 ) ) { \
						tiles->At( (_x), (_y) )->features |= Tile::F_XENOFUNGUS; \
						if ( (_y) > 1 ) { \
							FUNGUS3( (_x), (_y) - 2 ); \
						} \
						if ( (_y) > 0 ) { \
							FUNGUS3( (_x) > 0 ? (_x) - 1 : width - 1, (_y) - 1 ); \
							FUNGUS3( (_x) < width - 1 ? (_x) + 1 : 0, (_y) - 1 ); \
						} \
						FUNGUS3( (_x) > 1 ? (_x) - 2 : width - 1 - ( 1 - ( (_y) % 2 ) ), (_y) ); \
						FUNGUS3( (_x) < width - 2 ? (_x) + 2 : (_y) % 2, (_y) ); \
						if ( (_y) > 1 ) { \
							FUNGUS3( (_x), (_y) - 2 ); \
						} \
						if ( (_y) < height - 1 ) { \
							FUNGUS3( (_x) > 0 ? (_x) - 1 : width - 1, (_y) + 1 ); \
							FUNGUS3( (_x) < width - 1 ? (_x) + 1 : 0, (_y) + 1 ); \
						} \
						if ( (_y) < height - 2 ) { \
							FUNGUS3( (_x), (_y) + 2 ); \
						} \
					} \
				}

				#define FUNGUS1( _x, _y ) { \
					if ( tiles->GetRandom()->IsLucky( 2 ) ) { \
						tiles->At( (_x), (_y) )->features |= Tile::F_XENOFUNGUS; \
						if ( (_y) > 1 ) { \
							FUNGUS2( (_x), (_y) - 2 ); \
						} \
						if ( (_y) > 0 ) { \
							FUNGUS2( (_x) > 0 ? (_x) - 1 : width - 1, (_y) - 1 ); \
							FUNGUS2( (_x) < width - 1 ? (_x) + 1 : 0, (_y) - 1 ); \
						} \
						FUNGUS2( (_x) > 1 ? (_x) - 2 : width - 1 - ( 1 - ( (_y) % 2 ) ), (_y) ); \
						FUNGUS2( (_x) < width - 2 ? (_x) + 2 : (_y) % 2, (_y) ); \
						if ( (_y) > 1 ) { \
							FUNGUS2( (_x), (_y) - 2 ); \
						} \
						if ( (_y) < height - 1 ) { \
							FUNGUS2( (_x) > 0 ? (_x) - 1 : width - 1, (_y) + 1 ); \
							FUNGUS2( (_x) < width - 1 ? (_x) + 1 : 0, (_y) + 1 ); \
						} \
						if ( (_y) < height - 2 ) { \
							FUNGUS2( (_x), (_y) + 2 ); \
						} \
					} \
				}

				if ( y > 1 ) {
					FUNGUS1( x, y - 2 ); // N
				}
				if ( y > 0 ) {
					FUNGUS1( x > 0 ? x - 1 : width - 1, y - 1 ); // NW
					FUNGUS1( x < width - 1 ? x + 1 : 0, y - 1 ); // NE
				}
				FUNGUS1( x > 1 ? x - 2 : width - 1 - ( 1 - ( y % 2 ) ), y ); // W
				FUNGUS1( x < width - 2 ? x + 2 : y % 2, y ); // E
				if ( y > 1 ) {
					FUNGUS1( x, y - 2 ); // N
				}
				if ( y < height - 1 ) {
					FUNGUS1( x > 0 ? x - 1 : width - 1, y + 1 ); // SW
					FUNGUS1( x < width - 1 ? x + 1 : 0, y + 1 ); // SE
				}
				if ( y < height - 2 ) {
					FUNGUS1( x, y + 2 ); // S
				}
				#undef FUNGUS1
				#undef FUNGUS2
				#undef FUNGUS3
				#undef FUNGUS4
				
			}
		}
	}
	
#ifndef MAPGEN_BENCHMARK
	Finalize( tiles );
#endif
	
}

}
}
}

#endif
