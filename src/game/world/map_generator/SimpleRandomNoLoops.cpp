#include "SimpleRandomNoLoops.h"

#ifdef MAPGEN_BENCHMARK

// to check "pointers are slow" theory

namespace game {
namespace world {
namespace map_generator {

void SimpleRandomNoLoops::Generate( Tiles* tiles, size_t seed ) {
	Log( "Generating terrain ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );
	
	Tile* tile;
	
	const size_t width = tiles->GetWidth();
	const size_t height = tiles->GetHeight();
	
	for ( auto y = 0 ; y < height ; y++ ) {
		for ( auto x = y & 1 ; x < width ; x += 2 ) {
			tile = tiles->At( x, y );
		
#define RND MAPGEN_ELEVATION_MIN + tiles->GetRandom()->GetUInt( 0, MAPGEN_ELEVATION_MAX - MAPGEN_ELEVATION_MIN )
			
			*tile->elevation.top = RND;
			*tile->elevation.left = RND;
			*tile->elevation.right = RND;
			*tile->elevation.bottom = RND;
			
			// random moisture etc
			tile->moisture = tiles->GetRandom()->GetUInt( 1, 3 );
			tile->rockyness = tiles->GetRandom()->GetUInt( 1, 3 );
			tile->features = 0;
			
			// spawn some jungles
			if ( tiles->GetRandom()->IsLucky( 30 ) ) {
				tile->features |= Tile::F_JUNGLE;

				#define JUNGLES2( _tile ) { \
					if ( tiles->GetRandom()->IsLucky( 2 ) ) { \
						(_tile)->features |= Tile::F_JUNGLE; \
					} \
				}
				
				#define JUNGLES1( _tile ) { \
					if ( tiles->GetRandom()->IsLucky( 2 ) ) { \
						(_tile)->features |= Tile::F_JUNGLE; \
						if ( (_tile)->coord.y > 1 ) { \
							JUNGLES2( (_tile)->N ); \
						} \
						if ( (_tile)->coord.y > 0 ) { \
							JUNGLES2( (_tile)->NW ); \
							JUNGLES2( (_tile)->NE ); \
						} \
						JUNGLES2( (_tile)->W ); \
						JUNGLES2( (_tile)->E ); \
						if ( (_tile)->coord.y < height - 1 ) { \
							JUNGLES2( (_tile)->SW ); \
							JUNGLES2( (_tile)->SE ); \
						} \
						if ( (_tile)->coord.y < height - 2 ) { \
							JUNGLES2( (_tile)->S ); \
						} \
					} \
				}

				if ( tile->coord.y > 1 ) {
					JUNGLES1( tile->N );
				}
				if ( tile->coord.y > 0 ) {
					JUNGLES1( tile->NW );
					JUNGLES1( tile->NE );
				}
				JUNGLES1( tile->W );
				JUNGLES1( tile->E );
				if ( tile->coord.y < height - 1 ) {
					JUNGLES1( tile->SW );
					JUNGLES1( tile->SE );
				}
				if ( tile->coord.y < height - 2 ) {
					JUNGLES1( tile->S );
				}

				#undef JUNGLES1
				#undef JUNGLES2
			}
			
			if ( tiles->GetRandom()->IsLucky( 40 ) ) {
				
				tile->features |= Tile::F_XENOFUNGUS;
				
				#define FUNGUS4( _tile ) { \
					if ( tiles->GetRandom()->IsLucky( 5 ) ) { \
						(_tile)->features |= Tile::F_XENOFUNGUS; \
					} \
				}
				
				#define FUNGUS3( _tile ) { \
					if ( tiles->GetRandom()->IsLucky( 4 ) ) { \
						(_tile)->features |= Tile::F_XENOFUNGUS; \
						if ( (_tile)->coord.y > 1 ) { \
							FUNGUS4( (_tile)->N ); \
						} \
						if ( (_tile)->coord.y > 0 ) { \
							FUNGUS4( (_tile)->NW ); \
							FUNGUS4( (_tile)->NE ); \
						} \
						FUNGUS4( (_tile)->W ); \
						FUNGUS4( (_tile)->E ); \
						if ( (_tile)->coord.y < height - 1 ) { \
							FUNGUS4( (_tile)->SW ); \
							FUNGUS4( (_tile)->SE ); \
						} \
						if ( (_tile)->coord.y < height - 2 ) { \
							FUNGUS4( (_tile)->S ); \
						} \
					} \
				}

				#define FUNGUS2( _tile ) { \
					if ( tiles->GetRandom()->IsLucky( 3 ) ) { \
						(_tile)->features |= Tile::F_XENOFUNGUS; \
						if ( (_tile)->coord.y > 1 ) { \
							FUNGUS3( (_tile)->N ); \
						} \
						if ( (_tile)->coord.y > 0 ) { \
							FUNGUS3( (_tile)->NW ); \
							FUNGUS3( (_tile)->NE ); \
						} \
						FUNGUS3( (_tile)->W ); \
						FUNGUS3( (_tile)->E ); \
						if ( (_tile)->coord.y < height - 1 ) { \
							FUNGUS3( (_tile)->SW ); \
							FUNGUS3( (_tile)->SE ); \
						} \
						if ( (_tile)->coord.y < height - 2 ) { \
							FUNGUS3( (_tile)->S ); \
						} \
					} \
				}

				#define FUNGUS1( _tile ) { \
					if ( tiles->GetRandom()->IsLucky( 2 ) ) { \
						(_tile)->features |= Tile::F_XENOFUNGUS; \
						if ( (_tile)->coord.y > 1 ) { \
							FUNGUS2( (_tile)->N ); \
						} \
						if ( (_tile)->coord.y > 0 ) { \
							FUNGUS2( (_tile)->NW ); \
							FUNGUS2( (_tile)->NE ); \
						} \
						FUNGUS2( (_tile)->W ); \
						FUNGUS2( (_tile)->E ); \
						if ( (_tile)->coord.y < height - 1 ) { \
							FUNGUS2( (_tile)->SW ); \
							FUNGUS2( (_tile)->SE ); \
						} \
						if ( (_tile)->coord.y < height - 2 ) { \
							FUNGUS2( (_tile)->S ); \
						} \
					} \
				}

				if ( tile->coord.y > 1 ) {
					FUNGUS1( tile->N );
				}
				if ( tile->coord.y > 0 ) {
					FUNGUS1( tile->NW );
					FUNGUS1( tile->NE );
				}
				FUNGUS1( tile->W );
				FUNGUS1( tile->E );
				if ( tile->coord.y < height - 1 ) {
					FUNGUS1( tile->SW );
					FUNGUS1( tile->SE );
				}
				if ( tile->coord.y < height - 2 ) {
					FUNGUS1( tile->S );
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
