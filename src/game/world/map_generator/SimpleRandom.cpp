#include "SimpleRandom.h"

namespace game {
namespace world {
namespace map_generator {

void SimpleRandom::GenerateElevations( Tiles* tiles, const MapSettings& map_settings ) {
	Tile* tile;
	
	Log( "Generating elevations ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );
	
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
			tile = tiles->At( x, y );

#define RND MAPGEN_ELEVATION_MIN + m_random->GetUInt( 0, MAPGEN_ELEVATION_MAX - MAPGEN_ELEVATION_MIN )

			*tile->elevation.top = RND;
			*tile->elevation.left = RND;
			*tile->elevation.right = RND;
			*tile->elevation.bottom = RND;
		}
	}

}

void SimpleRandom::GenerateDetails( Tiles* tiles, const MapSettings& map_settings ) {
	Tile* tile;
	
	Log( "Generating details ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );

	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
			tile = tiles->At( x, y );

			// random moisture etc
			tile->moisture = m_random->GetUInt( Tile::M_ARID, Tile::M_MOIST );
			tile->rockiness = m_random->GetUInt( Tile::R_FLAT, Tile::R_ROCKY );
			tile->features = 0;

			// spawn some jungles
			if ( m_random->IsLucky( 30 ) ) {
				tile->features |= Tile::F_JUNGLE;
				for ( auto& t : tile->neighbours ) {
					if ( m_random->IsLucky( 2 ) ) {
						t->features |= Tile::F_JUNGLE;
						for ( auto& tt : t->neighbours ) {
							if ( m_random->IsLucky( 2 ) ) {
								tt->features |= Tile::F_JUNGLE;
							}
						}
					}
				}
			}

			// spawn some fungus areas
			if ( m_random->IsLucky( 40 ) ) {
				tile->features |= Tile::F_XENOFUNGUS;
				for ( auto& t : tile->neighbours ) {
					if ( m_random->IsLucky( 2 ) ) {
						t->features |= Tile::F_XENOFUNGUS;
						for ( auto& tt : t->neighbours ) {
							if ( m_random->IsLucky( 3 ) ) {
								tt->features |= Tile::F_XENOFUNGUS;
								for ( auto& ttt : tt->neighbours ) {
									if ( m_random->IsLucky( 4 ) ) {
										ttt->features |= Tile::F_XENOFUNGUS;
										for ( auto& tttt : ttt->neighbours ) {
											if ( m_random->IsLucky( 5 ) ) {
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
}

}
}
}
