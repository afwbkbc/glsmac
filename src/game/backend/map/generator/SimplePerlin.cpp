#include <cmath>

#include "SimplePerlin.h"

#include "util/random/Random.h"
#include "util/Perlin.h"
#include "util/Clamper.h"

#include "game/backend/map/tile/Tiles.h"

// higher values generate more interesting maps, at cost of longer map generation (isn't noticeable before 200 or so)
#define PERLIN_PASSES 128

#define RIVER_SPAWN_CHANCE_DIFFICULTY 12
#define RIVER_STARTING_LENGTH_MIN 8
#define RIVER_STARTING_LENGTH_MAX 16
#define RIVER_DIRECTION_CHANGE_CHANCE_DIFFICULTY 2
#define RIVER_SPLIT_CHANCE_DIFFICULTY 6
#define RIVER_JOIN_CHANCE_DIFFICULTY 12

#define RIVER_RANDOM_DIRECTION ( m_random->GetUInt( 0, ( tile->neighbours.size() - 1 ) ) )
#define RIVER_RANDOM_DIRECTION_DIAGONAL ( m_random->GetUInt( 0, 1 ) * 2 - 1 )

#define RESOURCE_SPAWN_CHANCE_DIFFICULTY 24

namespace game {
namespace backend {
namespace map {
namespace generator {

void SimplePerlin::GenerateElevations( tile::Tiles* tiles, const game::backend::settings::MapSettings* map_settings, MT_CANCELABLE ) {
	tile::Tile* tile;

	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();

	Log( "Generating elevations ( " + std::to_string( w ) + " x " + std::to_string( h ) + " )" );

	const auto seed = m_random->GetUInt();

	const float land_bias = 1.3f; // increase amount of land generated
	util::Clamper< float > perlin_to_elevation(
		{
			{ -1.0f - land_bias,    1.0f },
			{ MAPGEN_ELEVATION_MIN, MAPGEN_ELEVATION_MAX }
		}
	);

	util::Clamper< float > perlin_to_value(
		{ // to moisture or rockiness
			{ -1.0f, 1.0f },
			{ 1.0f,  3.0f }
		}
	);

	util::Perlin perlin( seed );

	MT_RETIF();

	// process in random order
	std::vector< tile::Tile* > randomtiles = GetTilesInRandomOrder( tiles, MT_C );
	MT_RETIF();

	for ( auto& tile : randomtiles ) {

#define PERLIN_S( _x, _y, _z, _scale ) perlin.Noise( (float) ( (float)_x ) * _scale, (float) ( (float)_y ) * _scale, _z * _scale, PERLIN_PASSES )
#define PERLIN( _x, _y, _z ) PERLIN_S( _x, _y, _z, 1.0f )

		*tile->elevation.left = *tile->elevation.top = *tile->elevation.right = *tile->elevation.bottom = *tile->elevation.center = 0;

		const float z_elevation = 0;

		*tile->elevation.left = perlin_to_elevation.Clamp( PERLIN( tile->coord.x, tile->coord.y + 0.5f, z_elevation ) );
		*tile->elevation.top = perlin_to_elevation.Clamp( PERLIN( tile->coord.x + 0.5f, tile->coord.y, z_elevation ) );
		*tile->elevation.right = perlin_to_elevation.Clamp( PERLIN( tile->coord.x + 1.0f, tile->coord.y + 0.5f, z_elevation ) );
		*tile->elevation.bottom = perlin_to_elevation.Clamp( PERLIN( tile->coord.x + 0.5f, tile->coord.y + 1.0f, z_elevation ) );

		tile->Update();

		MT_RETIF();
	}

	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tile = &tiles->At( x, y );

			const float z_rocks = m_random->GetFloat( 0.0f, 1.0f );
			const float z_moisture = m_random->GetFloat( 0.0f, 1.0f );
			const float z_jungle = m_random->GetFloat( 0.0f, 1.0f );
			const float z_xenofungus = m_random->GetFloat( 0.0f, 1.0f );

			// moisture
			tile->moisture = perlin_to_value.Clamp( ceil( PERLIN_S( x + 0.5f, y + 0.5f, z_moisture, 0.6f ) ) );
			if ( tile->moisture == tile::MOISTURE_RAINY ) {
				if ( PERLIN_S( x + 0.5f, y + 0.5f, z_jungle, 0.2f ) > 0.7 ) {
					tile->features |= tile::FEATURE_JUNGLE;
				}
			}

			// rockiness
			tile->rockiness = perlin_to_value.Clamp( round( PERLIN_S( x + 0.5f, y + 0.5f, z_rocks, 1.0f ) ) );
			if ( tile->rockiness == tile::ROCKINESS_ROCKY ) {
				if ( m_random->IsLucky( 3 ) ) {
					tile->rockiness = tile::ROCKINESS_ROLLING;
				}
			}
			// extra rockiness spots
			if ( m_random->IsLucky( 30 ) ) {
				tile->rockiness = tile::ROCKINESS_ROCKY;
				for ( auto& t : tile->neighbours ) {
					if ( m_random->IsLucky( 3 ) ) {
						if ( t->rockiness != tile::ROCKINESS_ROCKY ) {
							t->rockiness = tile::ROCKINESS_ROLLING;
						}
					}
				}
			}

			// fungus
			if ( PERLIN_S( x + 0.5f, y + 0.5f, z_xenofungus, 0.6f ) > 0.4 ) {
				tile->features |= tile::FEATURE_XENOFUNGUS;
			}

			MT_RETIF();
		}
	}

	for ( size_t i = 0 ; i < 8 ; i++ ) {
		// smooth land 2 times, water 8 times
		SmoothTerrain( tiles, MT_C, ( i < 2 ), true );
		MT_RETIF();
	}
}

void SimplePerlin::GenerateDetails( tile::Tiles* tiles, const game::backend::settings::MapSettings* map_settings, MT_CANCELABLE ) {
	tile::Tile* tile;

	Log( "Generating details ( " + std::to_string( tiles->GetWidth() ) + " x " + std::to_string( tiles->GetHeight() ) + " )" );

	// terrain-dependent features need to go after Finalize to make sure terrain elevations and properties won't change after
	// TODO: split generation into 2 methods
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
			tile = &tiles->At( x, y );

			// add some rivers
			if ( m_random->IsLucky( RIVER_SPAWN_CHANCE_DIFFICULTY ) ) {
				GenerateRiver(
					tiles,
					tile,
					m_random->GetUInt( RIVER_STARTING_LENGTH_MIN, RIVER_STARTING_LENGTH_MAX ),
					RIVER_RANDOM_DIRECTION,
					RIVER_RANDOM_DIRECTION_DIAGONAL,
					MT_C
				);
			}

			// bonus resources
			if ( m_random->IsLucky( RESOURCE_SPAWN_CHANCE_DIFFICULTY ) ) {
				tile->bonus = m_random->GetUInt( tile::BONUS_NUTRIENT, tile::BONUS_MINERALS );
			}

			MT_RETIF();
		}
	}
}

void SimplePerlin::GenerateRiver( tile::Tiles* tiles, tile::Tile* tile, uint8_t length, uint8_t direction, int8_t direction_diagonal, MT_CANCELABLE ) {

	if ( tile->features & tile::FEATURE_RIVER ) {
		// joined existing river
		return;
	}
	if ( tile->is_water_tile ) {
		// reached water
		return;
	}

	MT_RETIF();

	tile->features |= tile::FEATURE_RIVER;

	length--;
	if ( length > 0 ) {

		if ( m_random->IsLucky( RIVER_DIRECTION_CHANGE_CHANCE_DIFFICULTY ) ) {
			if ( m_random->IsLucky() ) {
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
			real_direction = (int8_t)direction + direction_diagonal;
			if ( real_direction < 0 ) {
				real_direction = tile->neighbours.size() - 1;
			}
			else if ( real_direction > tile->neighbours.size() - 1 ) {
				real_direction = 0;
			}
			direction_diagonal *= -1;
		}
		auto* selected_tile = tile->neighbours.at( real_direction );
		if ( !HasRiversNearby( tile, selected_tile ) || m_random->IsLucky( RIVER_JOIN_CHANCE_DIFFICULTY ) ) {
			GenerateRiver( tiles, selected_tile, length, real_direction, direction_diagonal, MT_C );
		}

		MT_RETIF();

		while ( m_random->IsLucky( RIVER_SPLIT_CHANCE_DIFFICULTY ) ) {
			// split at 90 degrees angle
			uint8_t child_direction = direction;
			if ( m_random->IsLucky() ) { // clockwise
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
			if ( !HasRiversNearby( tile, selected_tile ) || m_random->IsLucky( RIVER_JOIN_CHANCE_DIFFICULTY ) ) {
				GenerateRiver( tiles, selected_tile, length, child_direction, direction_diagonal * -1, MT_C );
			}

			MT_RETIF();
		}
	}
}

bool SimplePerlin::HasRiversNearby( tile::Tile* current_tile, tile::Tile* tile ) {
	for ( auto& t : tile->neighbours ) {
		if ( t != current_tile && t->features & tile::FEATURE_RIVER ) {
			return true;
		}
	}
	return false;
}

}
}
}
}
