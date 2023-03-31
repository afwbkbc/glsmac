#include <random>

#include "MapGenerator.h"
#include "util/Clamper.h"

#include "../map/Map.h"

#define SEA_LEVELS_COAST 0

namespace game {
namespace world {
namespace map_generator {

MapGenerator::MapGenerator( Random* random )
	: m_random( random )
{
	//
}

void MapGenerator::Generate( Tiles* tiles, const MapSettings& map_settings ) {
	ASSERT( TARGET_LAND_AMOUNTS.find( map_settings.ocean ) != TARGET_LAND_AMOUNTS.end(), "unknown map ocean setting " + std::to_string( map_settings.ocean ) );
	float desired_land_amount = TARGET_LAND_AMOUNTS.at( map_settings.ocean );
	
	bool need_generation = true;
	size_t regenerations_asked = 0;
	while ( need_generation ) {
		need_generation = false;
		
		tiles->Clear();
		
		GenerateElevations( tiles, map_settings );
	
		FixExtremeSlopes( tiles );
		NormalizeElevationRange( tiles );
	
		// n	ormalize oceans
		
		ASSERT( MAXIMUM_ACCEPTABLE_INACCURACY >= INITIAL_ACCEPTABLE_INACCURACY, "maximum acceptable inaccuracy smaller than initial, that would cause infinite loop" );
		
		float acceptable_inaccuracy = INITIAL_ACCEPTABLE_INACCURACY;
		
		do {
			if ( acceptable_inaccuracy > MAXIMUM_ACCEPTABLE_INACCURACY ) {
				regenerations_asked++;
				if ( regenerations_asked > MAXIMUM_REGENERATION_ATTEMPTS ) {
					Log( "Unable to achieve desired land amount of " + std::to_string( desired_land_amount ) + " in " + std::to_string( MAXIMUM_REGENERATION_ATTEMPTS ) + " tries, giving up" );
					THROW( "Map generator failed to generate acceptable state" );
				}
				else {
					Log( "Unable to achieve desired land amount of " + std::to_string( desired_land_amount ) + ", regenerating" );
					need_generation = true;
					break;
				}
			}
			SetLandAmount( tiles, desired_land_amount );
			NormalizeElevationRange( tiles );
			tiles->FixTopBottomRows( m_random );
			RemoveExtremeSlopes( tiles, Map::s_consts.tile.maximum_allowed_slope_elevation );
			acceptable_inaccuracy *= ACCEPTABLE_INACCURACY_CHANGE;
		} while ( fabs( GetLandAmount( tiles ) - desired_land_amount ) > acceptable_inaccuracy );
	}
	
	ASSERT( TARGET_EVELATION_MULTIPLIERS.find( map_settings.erosive ) != TARGET_EVELATION_MULTIPLIERS.end(), "unknown map erosive setting " + std::to_string( map_settings.erosive ) );
	
	// normalize erosive forces
	const auto range = GetElevationsRange( tiles );
	float target_elevation_multiplier = std::min< float >( 1.0f, (
		fabs( std::min< float >(
			(float) Tile::ELEVATION_MIN / range.first,
			(float) Tile::ELEVATION_MAX / range.second
		)) *
		TARGET_EVELATION_MULTIPLIERS.at( map_settings.erosive )
	));
	
	MultiplyAllTilesBy( tiles, target_elevation_multiplier );
	
	Log( "Final land amount: " + std::to_string( GetLandAmount( tiles ) ) );
#ifdef DEBUG
	{
		const auto range = GetElevationsRange( tiles );
		Log( "Final elevations range: " + std::to_string( range.first ) + " " + std::to_string( range.second ) );
	}
#endif
	
	GenerateDetails( tiles, map_settings );
}

void MapGenerator::SmoothTerrain( Tiles* tiles, const bool smooth_land, const bool smooth_sea ) {
	
	Tile::elevation_t c;
	Tile::elevation_t sharpest;
	int8_t idx;
	int8_t mod;
	Tile* tile;
	uint8_t i;
	for ( auto y = 0 ; y < tiles->GetHeight() ; y++ ) {
		for ( auto x = y & 1 ; x < tiles->GetWidth() ; x += 2 ) {
			tile = tiles->At( x, y );
			
			tile->Update();
			
			if (
				( tile->is_water_tile && !smooth_sea ) ||
				( !tile->is_water_tile && !smooth_land )
			) {
				continue;
			}
				
			mod = tile->is_water_tile ? -1 : 1;
			
			// flatten every corner
			for( auto& c : tile->elevation.corners ) {
				*c = ( *c + *tile->elevation.center ) / 2;
			}
			
		}
	}
}


void MapGenerator::FixExtremeSlopes( Tiles* tiles ) {
	auto elevations_range = GetElevationsRange( tiles );
	util::Clamper< Tile::elevation_t > converter({
		{ Tile::ELEVATION_MIN, Tile::ELEVATION_MAX },
		{ elevations_range.first, elevations_range.second }
	});
	RemoveExtremeSlopes( tiles, abs( converter.Clamp( map::Map::s_consts.tile.maximum_allowed_slope_elevation ) ) );
}

void MapGenerator::SetLandAmount( Tiles* tiles, const float amount ) {
	
	Log( "Setting land amount to " + std::to_string( amount ) );
	
	float acceptable_inaccuracy = 0.01f;
	Tile::elevation_t elevation = Tile::ELEVATION_LEVEL_COAST; // start with 0
	Tile::elevation_t elevation_change_by = ( Tile::ELEVATION_MAX - Tile::ELEVATION_MIN ) / 2 ; // for bisecting to find best elevation difference for desired land amount
	float current_land_amount;
	int8_t last_direction = 0;
	while ( elevation_change_by > 1 ) {
		current_land_amount = GetLandAmount( tiles, elevation );
		
		if ( current_land_amount >= amount - acceptable_inaccuracy && current_land_amount <= amount + acceptable_inaccuracy ) {
			// found optimal elevation
			break;
		}
		
		if ( current_land_amount < amount ) {
			elevation += elevation_change_by;
			if ( last_direction != 1 ) {
				last_direction = 1;
				elevation_change_by /= 2;
			}
		}
		else {
			elevation -= elevation_change_by;
			if ( last_direction != -1 ) {
				last_direction = -1;
				elevation_change_by /= 2;
			}
		}
	}
	
	RaiseAllTilesBy( tiles, elevation );
}

const float MapGenerator::GetLandAmount( Tiles* tiles, Tile::elevation_t elevation_diff ) {
	size_t land_tiles = 0;
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			if ( *tiles->At( x, y )->elevation.center > -elevation_diff ) {
				land_tiles++;
			}
		}
	}
	return (float) land_tiles / ( ( w * h ) / 2 ); // divide by 2 because of SMAC coordinate system (not every tile exists)
}

void MapGenerator::RaiseAllTilesBy( Tiles* tiles, Tile::elevation_t amount ) {
	Log( "Raising all tiles by " + std::to_string( amount ) );
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	Tile* tile;
	// process in random order
	std::vector< Tile* > randomtiles;
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			randomtiles.push_back( tiles->At( x, y ) );
		}
	}
	std::mt19937 g( m_random->GetUInt( 0, UINT32_MAX - 1 ) );
	std::shuffle( randomtiles.begin(), randomtiles.end(), g );
	
	for ( auto& tile : randomtiles ) {
		*tile->elevation.center += amount;
		*tile->elevation.bottom += amount;
	}
	
	for ( auto y = 0; y < 2 ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			if ( y == 0 ) {
				*tiles->TopRightVertexAt( x ) += amount;
			}
			*tiles->TopVertexAt( x, y ) += amount;
		}
	}
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tiles->At( x, y )->Update();
		}
	}
}

void MapGenerator::MultiplyAllTilesBy( Tiles* tiles, float amount ) {
	Log( "Multiplying all tiles by " + std::to_string( amount ) );
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	Tile* tile;
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tile = tiles->At( x, y );
			*tile->elevation.center *= amount;
			*tile->elevation.bottom *= amount;
		}
	}
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tiles->At( x, y )->Update();
		}
	}
}

const std::pair< Tile::elevation_t, Tile::elevation_t > MapGenerator::GetElevationsRange( Tiles* tiles ) const {
	std::pair< Tile::elevation_t, Tile::elevation_t > result = { 0, 0 };
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	Tile* tile;
	// determine min and max elevations from generated tiles
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tile = tiles->At( x, y );
			for ( auto& c : tile->elevation.corners ) {
				if ( *c < result.first ) {
					result.first = *c;
				}
				else if ( *c > result.second ) {
					result.second = *c;
				}
			}
		}
	}
	//Log( "Elevations range: min=" + std::to_string( result.first ) + " max=" + std::to_string( result.second ) );
	return result;
}

void MapGenerator::RemoveExtremeSlopes( Tiles* tiles, const Tile::elevation_t max_allowed_diff ) {
	Tile::elevation_t elevation_fixby_change = 1;
	Tile::elevation_t elevation_fixby_max = max_allowed_diff / 3; // to prevent infinite loops when it grows so large it starts creating new extreme slopes
	float elevation_fixby_div_change = 0.001f; // needed to prevent infinite loops when nearby tiles keep 'fixing' each other forever
	
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	
	size_t pass = 0;
	Tile* tile;
	Tile::elevation_t elevation_fixby = 0;
	float elevation_fixby_div = 1.0f;
	bool found = true;
	size_t i;

	// process in random order
	std::vector< Tile* > randomtiles;
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			randomtiles.push_back( tiles->At( x, y ) );
		}
	}

	Log( "Checking/fixing extreme slopes" );
	
	while ( found ) {
		if ( elevation_fixby < elevation_fixby_max ) {
			elevation_fixby += elevation_fixby_change;
		}
		elevation_fixby_div += elevation_fixby_div_change;
		//Log( "Checking/fixing extreme slopes (pass " + std::to_string( ++pass ) + ")" );
		found = false;
		
		std::mt19937 g( m_random->GetUInt( 0, UINT32_MAX - 1 ) );
		std::shuffle( randomtiles.begin(), randomtiles.end(), g );
	
		// don't run in normal cycle because it can give terrain some straight edges, go in random order instead
		// assume that on average we'll hit all tiles (but skipping some is no big deal)
		for ( i = 0 ; i < randomtiles.size() ; i++ ) {
			tile = randomtiles[ i ];

			#define x( _a, _b ) \
				if ( abs( *tile->elevation._a - *tile->elevation._b ) > max_allowed_diff ) { \
					/*Log( "fixing slope: " + std::to_string( *tile->elevation._a ) + "," + std::to_string( *tile->elevation._b ) + " / " + std::to_string( elevation_fixby ) );*/ \
					*tile->elevation._a += ( *tile->elevation._a < *tile->elevation._b ) ? elevation_fixby : -elevation_fixby; \
					*tile->elevation._b += ( *tile->elevation._b < *tile->elevation._a ) ? elevation_fixby : -elevation_fixby; \
					*tile->elevation._a /= elevation_fixby_div; \
					*tile->elevation._b /= elevation_fixby_div; \
					found = true; \
				}
				x( left, right );
				x( left, top );
				x( left, bottom );
				x( right, top );
				x( right, bottom );
				x( top, bottom );
			#undef x

			if ( found ) {
				tile->Update();
			}
		}
	}
}

void MapGenerator::NormalizeElevationRange( Tiles* tiles ) {
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	auto elevations_range = GetElevationsRange( tiles );
	util::Clamper< Tile::elevation_t > converter({
		{ elevations_range.first, elevations_range.second },
		{ Tile::ELEVATION_MIN, Tile::ELEVATION_MAX }
	});
	
	// process in random order
	std::vector< Tile* > randomtiles;
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			randomtiles.push_back( tiles->At( x, y ) );
		}
	}
	std::mt19937 g( m_random->GetUInt( 0, UINT32_MAX - 1 ) );
	std::shuffle( randomtiles.begin(), randomtiles.end(), g );
	
	for ( auto& tile : randomtiles ) {
		tile->elevation_data.bottom = converter.Clamp( tile->elevation_data.bottom );
		tile->elevation_data.center = converter.Clamp( tile->elevation_data.center );
	}

	// convert top rows too
	for ( auto y = 0; y < 2 ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			if ( y == 0 ) {
				*tiles->TopRightVertexAt( x ) = converter.Clamp( *tiles->TopRightVertexAt( x ) );
			}
			*tiles->TopVertexAt( x, y ) = converter.Clamp( *tiles->TopVertexAt( x, y ) );
		}
	}
}

}
}
}
