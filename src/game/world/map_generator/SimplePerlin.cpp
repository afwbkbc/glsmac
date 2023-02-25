#include <ctime>
#include <cmath>
#include <algorithm>
#include <random>

#include "SimplePerlin.h"

#include "util/Perlin.h"
#include "util/Clamper.h"

// higher values generate more interesting maps, at cost of longer map generation (isn't noticeable before 200 or so)
#define PERLIN_PASSES 128

#define RIVER_SPAWN_CHANCE_DIFFICULTY 16
#define RIVER_STARTING_LENGTH_MIN 3
#define RIVER_STARTING_LENGTH_MAX 24
#define RIVER_DIRECTION_CHANGE_CHANCE_DIFFICULTY 2
#define RIVER_SPLIT_CHANCE_DIFFICULTY 6
#define RIVER_JOIN_CHANCE_DIFFICULTY 6

#define RANDOM_DIRECTION ( tiles->GetRandom()->GetUInt( 0, ( tile->neighbours.size() - 1 ) ) )
#define RANDOM_DIRECTION_DIAGONAL ( tiles->GetRandom()->GetUInt( 0, 1 ) * 2 - 1 )

namespace game {
namespace world {
namespace map_generator {

void SimplePerlin::Generate( Tiles* tiles, size_t seed ) {
	Log( "Generating terrain ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );
	
	const float land_bias = 1.3f; // increase amount of land generated
	util::Clamper<float> perlin_to_elevation( -1.0 - land_bias, 1.0, MAPGEN_ELEVATION_MIN, MAPGEN_ELEVATION_MAX );
	
	util::Clamper<float> perlin_to_value; // to moisture or rockyness
	perlin_to_value.SetRange( -1.0, 1.0, 1, 3 );
	
	util::Perlin perlin( seed );
	
	Tile* tile;
	
	// process in random order
	std::vector< Tile* > tiles_vec;
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
			tiles_vec.push_back( tiles->At( x, y ) );
		}
	}

	std::mt19937 g( seed );
	std::shuffle( tiles_vec.begin(), tiles_vec.end(), g );
	
	for ( auto& tile : tiles_vec ) {
		
#define PERLIN_S( _x, _y, _z, _scale ) perlin.Noise( (float) ( (float)_x ) * _scale, (float) ( (float)_y ) * _scale, _z * _scale, PERLIN_PASSES )
#define PERLIN( _x, _y, _z ) PERLIN_S( _x, _y, _z, 1.0f )
		
		*tile->elevation.left = *tile->elevation.top = *tile->elevation.right = *tile->elevation.bottom = *tile->elevation.center = 0;
		
		const float z_elevation = 0;
		
		*tile->elevation.left = perlin_to_elevation.Clamp( PERLIN( tile->coord.x, tile->coord.y + 0.5f, z_elevation ) );
		*tile->elevation.top = perlin_to_elevation.Clamp( PERLIN( tile->coord.x + 0.5f, tile->coord.y, z_elevation ) );
		*tile->elevation.right = perlin_to_elevation.Clamp( PERLIN( tile->coord.x + 1.0f, tile->coord.y + 0.5f, z_elevation ) );
		*tile->elevation.bottom = perlin_to_elevation.Clamp( PERLIN( tile->coord.x + 0.5f, tile->coord.y + 1.0f, z_elevation ) );
		
		tile->Update();
	}

	// start new cycle because we want all tiles have updated averages and dynamic properties
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
			tile = tiles->At( x, y );

			const float z_rocks = 1/70;
			const float z_moisture = 1/20;
			const float z_jungle = 1/130;
			const float z_xenofungus = 1/240;

			// moisture
			tile->moisture = perlin_to_value.Clamp( ceil( PERLIN_S( x + 0.5f, y + 0.5f, z_moisture, 0.6f ) ) );
			if ( tile->moisture == Tile::M_RAINY ) {
				if ( PERLIN_S( x + 0.5f, y + 0.5f, z_jungle, 0.2f ) > 0.7 ) {
					tile->features |= Tile::F_JUNGLE;
				}
			}
			
			// rockyness
			tile->rockyness = perlin_to_value.Clamp( round( PERLIN_S( x + 0.5f, y + 0.5f, z_rocks, 1.0f ) ) );
			if ( tile->rockyness == Tile::R_ROCKY ) {
				if ( tiles->GetRandom()->IsLucky( 3 ) ) {
					tile->rockyness = Tile::R_ROLLING;
				}
			}
			// extra rockyness spots
			if ( tiles->GetRandom()->IsLucky( 30 ) ) {
				tile->rockyness = Tile::R_ROCKY;
				for ( auto& t : tile->neighbours ) {
					if ( tiles->GetRandom()->IsLucky( 3 ) ) {
						if ( t->rockyness != Tile::R_ROCKY ) {
							t->rockyness = Tile::R_ROLLING;
						}
					}
				}
			}
			
			// fungus
			if ( PERLIN_S( x + 0.5f, y + 0.5f, z_xenofungus, 0.6f ) > 0.4 ) {
				tile->features |= Tile::F_XENOFUNGUS;
			}

			// add some rivers
			if ( tiles->GetRandom()->IsLucky( RIVER_SPAWN_CHANCE_DIFFICULTY ) ) {
				GenerateRiver(
					tiles,
					tile,
					tiles->GetRandom()->GetUInt( RIVER_STARTING_LENGTH_MIN, RIVER_STARTING_LENGTH_MAX ),
					RANDOM_DIRECTION,
					RANDOM_DIRECTION_DIAGONAL
				);
			}
			
		}
	}
	
	for ( size_t i = 0 ; i < 8 ; i++ ) {
		SmoothTerrain( tiles, ( i < 2 ), true ); // smooth land 2 times, sea 8 times
	}
	
	Finalize( tiles );
	
}

void SimplePerlin::GenerateRiver( Tiles* tiles, Tile* tile, uint8_t length, uint8_t direction, int8_t direction_diagonal ) {
	
	if ( tile->features & Tile::F_RIVER ) {
		// joined existing river
		return;
	}
	
	tile->features |= Tile::F_RIVER;
	
	length--;
	if ( length > 0 ) {
		
		if ( tiles->GetRandom()->IsLucky( RIVER_DIRECTION_CHANGE_CHANCE_DIFFICULTY ) )  {
			if ( tiles->GetRandom()->IsLucky() ) {
				if ( direction < tile->neighbours.size() - 1 ) {
					direction++;
				}
				else {
					direction = 0;
				}
			}
			else {
				if ( direction > 0 ) {
					direction--;
				}
				else {
					direction = tile->neighbours.size() - 1;
				}
			}
		}
		
		int8_t real_direction;
		if ( direction % 2 ) {
			real_direction = direction;
		}
		else {
			real_direction = direction + direction_diagonal;
			if ( real_direction < 0 ) {
				real_direction = tile->neighbours.size() - 1;
			}
			else if ( real_direction > tile->neighbours.size() - 1 ) {
				real_direction = 0;
			}
			direction_diagonal *= -1;
		}
		auto* selected_tile = tile->neighbours.at( real_direction );
		if ( !HasRiversNearby( tile, selected_tile ) || tiles->GetRandom()->IsLucky( RIVER_JOIN_CHANCE_DIFFICULTY ) ) {
			GenerateRiver( tiles, selected_tile, length, real_direction, direction_diagonal );
		}
		
		while ( tiles->GetRandom()->IsLucky( RIVER_SPLIT_CHANCE_DIFFICULTY ) ) {
			// split at 90 degrees angle
			uint8_t child_direction = direction;
			if ( tiles->GetRandom()->IsLucky() ) { // clockwise
				if ( child_direction < tile->neighbours.size() - 2 ) {
					child_direction += 2;
				}
				else {
					child_direction = child_direction + 2 - tile->neighbours.size();
				}
			}
			else { // counter-clockwise
				if ( child_direction >= 2 ) {
					child_direction -= 2;
				}
				else {
					child_direction = tile->neighbours.size() - child_direction - 1;
				}
			}
			
			selected_tile = tile->neighbours.at( child_direction );
			if ( !HasRiversNearby( tile, selected_tile ) || tiles->GetRandom()->IsLucky( RIVER_JOIN_CHANCE_DIFFICULTY ) ) {
				GenerateRiver( tiles, selected_tile, length, child_direction, direction_diagonal * -1 );
			}
		}
	}
}

bool SimplePerlin::HasRiversNearby( Tile* current_tile, Tile* tile ) {
	for ( auto& t : tile->neighbours ) {
		if ( t != current_tile && t->features & Tile::F_RIVER ) {
			return true;
		}
	}
	return false;
}

}
}
}
