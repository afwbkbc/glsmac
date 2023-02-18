#include "SimpleRandom.h"

namespace game {
namespace world {
namespace map_generator {

void SimpleRandom::Generate( Tiles* tiles, size_t seed ) {
	Log( "Generating terrain ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );
	
	Tile* tile;
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
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
				for ( auto& t : tile->neighbours ) {
					if ( tiles->GetRandom()->IsLucky( 2 ) ) {
						t->features |= Tile::F_JUNGLE;
						for ( auto& tt : t->neighbours ) {
							if ( tiles->GetRandom()->IsLucky( 2 ) ) {
								tt->features |= Tile::F_JUNGLE;
							}
						}
					}
				}
			}
			
			// spawn some fungus areas
			if ( tiles->GetRandom()->IsLucky( 40 ) ) {
				tile->features |= Tile::F_XENOFUNGUS;
				for ( auto& t : tile->neighbours ) {
					if ( tiles->GetRandom()->IsLucky( 2 ) ) {
						t->features |= Tile::F_XENOFUNGUS;
						for ( auto& tt : t->neighbours ) {
							if ( tiles->GetRandom()->IsLucky( 3 ) ) {
								tt->features |= Tile::F_XENOFUNGUS;
								for ( auto& ttt : tt->neighbours ) {
									if ( tiles->GetRandom()->IsLucky( 4 ) ) {
										ttt->features |= Tile::F_XENOFUNGUS;
										for ( auto& tttt : ttt->neighbours ) {
											if ( tiles->GetRandom()->IsLucky( 5 ) ) {
												tttt->features |= Tile::F_XENOFUNGUS;
											}
										}
									}
								}
							}
						}
					}
				}
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
