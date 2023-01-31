#include "SimpleRandom.h"

namespace game {
namespace world {
namespace map_generator {

void SimpleRandom::Generate( Tiles* tiles ) {
	Log( "Generating terrain ( " + to_string( tiles->GetWidth() ) + " x " + to_string( tiles->GetHeight() ) + " )" );
	
	Tile* tile;
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = 0 ; x < tiles->GetWidth() ; x++ ) {
			if ( ( y % 2 ) != ( x % 2 ) ) {
				continue;
			}
			
			tile = tiles->At( x, y );
		
#define RND Tile::ELEVATION_MIN + rand() % ( Tile::ELEVATION_MAX - Tile::ELEVATION_MIN + 1 )
			
			*tile->elevation.top = RND;
			*tile->elevation.left = RND;
			*tile->elevation.right = RND;
			*tile->elevation.bottom = RND;
			
			// random moisture etc
			tile->moisture = rand() % 3 + 1;
			tile->rockyness = rand() % 3 + 1;
			tile->features = 0;
			
			// spawn some jungles
			if ( rand() % 30 == 0 ) {
				tile->features |= Tile::F_JUNGLE;
				for ( auto& t : tile->neighbours ) {
					if ( rand() % 2 == 0 ) {
						t->features |= Tile::F_JUNGLE;
						for ( auto& tt : t->neighbours ) {
							if ( rand() % 2 == 0 ) {
								tt->features |= Tile::F_JUNGLE;
							}
						}
					}
				}
			}
			
			// spawn some fungus areas
			if ( rand() % 40 == 0 ) {
				tile->features |= Tile::F_XENOFUNGUS;
				for ( auto& t : tile->neighbours ) {
					if ( rand() % 2 == 0 ) {
						t->features |= Tile::F_XENOFUNGUS;
						for ( auto& tt : t->neighbours ) {
							if ( rand() % 3 == 0 ) {
								tt->features |= Tile::F_XENOFUNGUS;
								for ( auto& ttt : tt->neighbours ) {
									if ( rand() % 4 == 0 ) {
										ttt->features |= Tile::F_XENOFUNGUS;
										for ( auto& tttt : ttt->neighbours ) {
											if ( rand() % 5 == 0 ) {
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
	
	Finalize( tiles );
	
}

}
}
}
