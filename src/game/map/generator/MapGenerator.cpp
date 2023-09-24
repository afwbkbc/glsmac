#include "MapGenerator.h"

#include "util/Clamper.h"
#include "../Map.h"
#include "engine/Engine.h"

namespace game {
namespace map {
namespace generator {

MapGenerator::MapGenerator( Random* random )
	: m_random( random ) {
	//
}

void MapGenerator::Generate( Tiles* tiles, const MapSettings& map_settings, MT_CANCELABLE ) {
	ASSERT( TARGET_LAND_AMOUNTS.find( map_settings.ocean ) != TARGET_LAND_AMOUNTS.end(), "unknown map ocean setting " + std::to_string( map_settings.ocean ) );
	float desired_land_amount = TARGET_LAND_AMOUNTS.at( map_settings.ocean );

	auto* loader = g_engine->GetUI()->GetLoader();

	bool need_generation = true;
	size_t regenerations_asked = 0;
	while ( need_generation ) {
		need_generation = false;

		tiles->Clear();
		MT_RETIF();

		loader->SetText( "Generating elevations" );
		GenerateElevations( tiles, map_settings, MT_C );
		MT_RETIF();

		loader->SetText( "Normalizing elevations" );
		FixExtremeSlopes( tiles, MT_C );
		MT_RETIF();
		NormalizeElevationRange( tiles, MT_C );
		MT_RETIF();

		// n	ormalize oceans

		ASSERT( MAXIMUM_ACCEPTABLE_INACCURACY >= INITIAL_ACCEPTABLE_INACCURACY, "maximum acceptable inaccuracy smaller than initial, that would cause infinite loop" );

		float acceptable_inaccuracy = INITIAL_ACCEPTABLE_INACCURACY;

		loader->SetText( "Normalizing land amount" );
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
			SetLandAmount( tiles, desired_land_amount, MT_C );
			MT_RETIF();
			NormalizeElevationRange( tiles, MT_C );
			MT_RETIF();
			tiles->FixTopBottomRows( m_random );
			MT_RETIF();
			RemoveExtremeSlopes( tiles, s_consts.tile.maximum_allowed_slope_elevation, MT_C );
			MT_RETIF();
			acceptable_inaccuracy *= ACCEPTABLE_INACCURACY_CHANGE;
		}
		while ( fabs( GetLandAmount( tiles, MT_C ) - desired_land_amount ) > acceptable_inaccuracy );
		MT_RETIF();
	}

	loader->SetText( "Normalizing erosive forces" );
	// normalize erosive forces
	ASSERT( TARGET_EVELATION_MULTIPLIERS.find( map_settings.erosive ) != TARGET_EVELATION_MULTIPLIERS.end(), "unknown map erosive setting " + std::to_string( map_settings.erosive ) );
	const auto range = GetElevationsRange( tiles, MT_C );
	MT_RETIF();
	float target_elevation_multiplier = std::min< float >(
		1.0f, (
			fabs(
				std::min< float >(
					(float)Tile::ELEVATION_MIN / range.first,
					(float)Tile::ELEVATION_MAX / range.second
				)
			) *
				TARGET_EVELATION_MULTIPLIERS.at( map_settings.erosive )
		)
	);
	ScaleAllTilesBy( tiles, target_elevation_multiplier, MT_C );
	MT_RETIF();

	GenerateDetails( tiles, map_settings, MT_C );
	MT_RETIF();

	loader->SetText( "Normalizing fungus amount" );
	// normalize fungus amount
	ASSERT( TARGET_FUNGUS_AMOUNTS.find( map_settings.lifeforms ) != TARGET_FUNGUS_AMOUNTS.end(), "unknown map lifeforms setting " + std::to_string( map_settings.lifeforms ) );
	const auto desired_fungus_amount = TARGET_FUNGUS_AMOUNTS.at( map_settings.lifeforms );
	SetFungusAmount( tiles, desired_fungus_amount, MT_C );
	MT_RETIF();

	loader->SetText( "Normalizing moisture amount" );
	// normalize moisture amount
	ASSERT( TARGET_MOISTURE_AMOUNTS.find( map_settings.clouds ) != TARGET_MOISTURE_AMOUNTS.end(), "unknown map clouds setting " + std::to_string( map_settings.clouds ) );
	const auto desired_moisture_amount = TARGET_MOISTURE_AMOUNTS.at( map_settings.clouds );
	SetMoistureAmount( tiles, desired_moisture_amount, MT_C );
	MT_RETIF();

	loader->SetText( "Fixing impossible tiles" );
	FixImpossibleThings( tiles, MT_C );
	MT_RETIF();

	Log( "Final land amount: " + std::to_string( GetLandAmount( tiles, MT_C ) ) );
#ifdef DEBUG
	{
		const auto range = GetElevationsRange( tiles, MT_C );
		MT_RETIF();
		Log( "Final elevations range: " + std::to_string( range.first ) + " " + std::to_string( range.second ) );
	}
#endif
	Log( "Final fungus amount: " + std::to_string( GetFungusAmount( tiles, MT_C ) ) );
	Log( "Final moisture amount: " + std::to_string( GetMoistureAmount( tiles, MT_C ) ) );

	loader->SetText( "Map generation complete" );
}

void MapGenerator::SmoothTerrain( Tiles* tiles, MT_CANCELABLE, const bool smooth_land, const bool smooth_sea ) {

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

			mod = tile->is_water_tile
				? -1
				: 1;

			// flatten every corner
			for ( auto& c : tile->elevation.corners ) {
				*c = ( *c + *tile->elevation.center ) / 2;
			}

			MT_RETIF();
		}
	}
}

void MapGenerator::FixExtremeSlopes( Tiles* tiles, MT_CANCELABLE ) {
	auto elevations_range = GetElevationsRange( tiles, MT_C );
	MT_RETIF();
	util::Clamper< Tile::elevation_t > converter(
		{
			{ Tile::ELEVATION_MIN, Tile::ELEVATION_MAX },
			{ elevations_range.first, elevations_range.second }
		}
	);
	RemoveExtremeSlopes( tiles, abs( converter.Clamp( s_consts.tile.maximum_allowed_slope_elevation ) ), MT_C );
}

void MapGenerator::SetLandAmount( Tiles* tiles, const float amount, MT_CANCELABLE ) {

	Log( "Setting land amount to " + std::to_string( amount ) );

	float acceptable_inaccuracy = 0.01f;
	Tile::elevation_t elevation = Tile::ELEVATION_LEVEL_COAST; // start with 0
	Tile::elevation_t elevation_change_by = ( Tile::ELEVATION_MAX - Tile::ELEVATION_MIN ) / 2; // for bisecting to find best elevation difference for desired land amount
	float current_land_amount;
	int8_t last_direction = 0;
	while ( elevation_change_by > 1 ) {
		current_land_amount = GetLandAmount( tiles, MT_C, elevation );
		MT_RETIF();

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
		MT_RETIF();
	}

	RaiseAllTilesBy( tiles, elevation, MT_C );
}

const float MapGenerator::GetLandAmount( Tiles* tiles, MT_CANCELABLE, Tile::elevation_t elevation_diff ) {
	size_t land_tiles = 0;
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			if ( *tiles->At( x, y )->elevation.center > -elevation_diff ) {
				land_tiles++;
			}
			MT_RETIFV( 0.0f );
		}
	}
	return (float)land_tiles / ( w * h / 2 );
}

void MapGenerator::SetFungusAmount( Tiles* tiles, const float amount, MT_CANCELABLE ) {

	Tile* tile;
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	const auto sz = w * h / 2;

	std::vector< Tile* > with_fungus = {};
	std::vector< Tile* > without_fungus = {};

	// to avoid reallocations
	with_fungus.reserve( sz );
	without_fungus.reserve( sz );

	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tile = tiles->At( x, y );
			if ( tile->features & Tile::F_XENOFUNGUS ) {
				with_fungus.push_back( tile );
			}
			else {
				without_fungus.push_back( tile );
			}
			MT_RETIF();
		}
	}
	const size_t desired_fungus_tiles_count = round( amount * sz );
	if ( with_fungus.size() < desired_fungus_tiles_count ) {
		const auto c = desired_fungus_tiles_count - with_fungus.size();
		Log( "Adding fungus to " + std::to_string( c ) + " tiles" );
		m_random->Shuffle( without_fungus );
		for ( auto i = 0 ; i < c ; i++ ) {
			without_fungus[ i ]->features |= Tile::F_XENOFUNGUS;
			MT_RETIF();
		}
	}
	else if ( with_fungus.size() > desired_fungus_tiles_count ) {
		const auto c = with_fungus.size() - desired_fungus_tiles_count;
		Log( "Removing fungus from " + std::to_string( c ) + " tiles" );
		m_random->Shuffle( with_fungus );
		for ( auto i = 0 ; i < c ; i++ ) {
			with_fungus[ i ]->features &= ~Tile::F_XENOFUNGUS;
			MT_RETIF();
		}
	}
}

const float MapGenerator::GetFungusAmount( Tiles* tiles, MT_CANCELABLE ) {
	size_t fungus_tiles = 0;
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			if ( tiles->At( x, y )->features & Tile::F_XENOFUNGUS ) {
				fungus_tiles++;
			}
			MT_RETIFV( 0.0f );
		}
	}
	return (float)fungus_tiles / ( w * h / 2 );
}

void MapGenerator::SetMoistureAmount( Tiles* tiles, const float amount, MT_CANCELABLE ) {
	Tile* tile;
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	const auto sz = w * h / 2;

	float moisture_amount = 0.0f;
	std::vector< Tile* > arid_tiles = {};
	std::vector< Tile* > moist_tiles = {};
	std::vector< Tile* > rainy_tiles = {};

	// to avoid reallocations
	arid_tiles.reserve( sz );
	moist_tiles.reserve( sz );
	rainy_tiles.reserve( sz );

	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tile = tiles->At( x, y );
			switch ( tile->moisture ) {
				case Tile::M_ARID: {
					arid_tiles.push_back( tile );
					break;
				}
				case Tile::M_MOIST: {
					moisture_amount += 0.5f;
					moist_tiles.push_back( tile );
					break;
				}
				case Tile::M_RAINY: {
					moisture_amount += 1.0f;
					rainy_tiles.push_back( tile );
					break;
				}
				default: {
					ASSERT( false, "unknown moisture value" );
				}
			}
			MT_RETIF();
		}
	}

	const float desired_moisture_amount = amount * sz;
	if ( moisture_amount < desired_moisture_amount ) {
		Log( "Adding moisture to " + std::to_string( desired_moisture_amount - moisture_amount ) + " tiles" ); // approximate
		m_random->Shuffle( arid_tiles );
		MT_RETIF();
		size_t i_arid = 0;
		m_random->Shuffle( moist_tiles );
		MT_RETIF();
		size_t i_moist = 0;
		bool use_moist = false;
		std::vector< Tile* > new_moist_tiles = {};
		new_moist_tiles.reserve( arid_tiles.size() );
		while ( moisture_amount < desired_moisture_amount ) {

			if ( i_arid < arid_tiles.size() && i_moist < moist_tiles.size() ) {
				use_moist = m_random->GetBool();
			}
			else if ( i_arid < arid_tiles.size() ) {
				use_moist = false;
			}
			else if ( i_moist < moist_tiles.size() ) {
				use_moist = true;
			}
			else {
				break; // exceeded
			}
			if ( use_moist ) {
				moist_tiles[ i_moist++ ]->moisture = Tile::M_RAINY;
			}
			else {
				arid_tiles[ i_arid ]->moisture = Tile::M_MOIST;
				new_moist_tiles.push_back( arid_tiles[ i_arid ] ); // to make rainy later if needed
				i_arid++;
			}
			moisture_amount += 0.5f;

			MT_RETIF();
		}
		if ( moisture_amount < desired_moisture_amount ) {
			// use new moist tiles if still need to increase moisture
			i_moist = 0;
			while ( moisture_amount < desired_moisture_amount ) {
				ASSERT( i_moist < new_moist_tiles.size(), "unable to add enough moisture" );
				new_moist_tiles[ i_moist++ ]->moisture = Tile::M_RAINY;
				moisture_amount += 0.5f;
			}
		}
	}
	else if ( moisture_amount > desired_moisture_amount ) {
		Log( "Removing moisture from " + std::to_string( moisture_amount - desired_moisture_amount ) + " tiles" ); // approximate
		m_random->Shuffle( rainy_tiles );
		MT_RETIF();
		size_t i_rainy = 0;
		m_random->Shuffle( moist_tiles );
		MT_RETIF();
		size_t i_moist = 0;
		bool use_moist = false;
		std::vector< Tile* > new_moist_tiles = {};
		new_moist_tiles.reserve( rainy_tiles.size() );
		while ( moisture_amount > desired_moisture_amount ) {

			if ( i_rainy < rainy_tiles.size() && i_moist < moist_tiles.size() ) {
				use_moist = m_random->GetBool();
			}
			else if ( i_rainy < rainy_tiles.size() ) {
				use_moist = false;
			}
			else if ( i_moist < moist_tiles.size() ) {
				use_moist = true;
			}
			else {
				break; // exceeded
			}
			if ( use_moist ) {
				moist_tiles[ i_moist++ ]->moisture = Tile::M_ARID;
			}
			else {
				rainy_tiles[ i_rainy ]->moisture = Tile::M_MOIST;
				new_moist_tiles.push_back( rainy_tiles[ i_rainy ] ); // to make arid later if needed
				i_rainy++;
			}
			moisture_amount -= 0.5f;

			MT_RETIF();
		}
		if ( moisture_amount > desired_moisture_amount ) {
			// use new moist tiles if still need to decrease moisture
			i_moist = 0;
			while ( moisture_amount > desired_moisture_amount ) {
				ASSERT( i_moist < new_moist_tiles.size(), "unable to remove enough moisture" );
				new_moist_tiles[ i_moist++ ]->moisture = Tile::M_ARID;
				moisture_amount -= 0.5f;
			}
		}
	}
}

const float MapGenerator::GetMoistureAmount( Tiles* tiles, MT_CANCELABLE ) {
	float moisture_amount = 0.0f;
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			switch ( tiles->At( x, y )->moisture ) {
				case Tile::M_ARID: {
					break;
				}
				case Tile::M_MOIST: {
					moisture_amount += 0.5f;
					break;
				}
				case Tile::M_RAINY: {
					moisture_amount += 1.0f;
					break;
				}
				default: {
					ASSERT( false, "unknown moisture value" );
				}
			}

			MT_RETIFV( 0.0f );
		}
	}
	return moisture_amount / ( w * h / 2 );

}

void MapGenerator::FixImpossibleThings( Tiles* tiles, MT_CANCELABLE ) {
	Tile* tile;
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tile = tiles->At( x, y );
			if ( tile->features & Tile::F_JUNGLE && tile->moisture != Tile::M_RAINY ) {
				// jungle should only be on rainy tiles
				tile->features &= ~Tile::F_JUNGLE;
			}
			MT_RETIF();
		}
	}
}

const std::vector< Tile* > MapGenerator::GetTilesInRandomOrder( const Tiles* tiles, MT_CANCELABLE ) {
	std::vector< Tile* > randomtiles;
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			randomtiles.push_back( tiles->At( x, y ) );
			MT_RETIFV( {} );
		}
	}
	m_random->Shuffle( randomtiles );
	return randomtiles;
}

void MapGenerator::RaiseAllTilesBy( Tiles* tiles, Tile::elevation_t amount, MT_CANCELABLE ) {
	Log( "Raising all tiles by " + std::to_string( amount ) );
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	Tile* tile;

	const auto randomtiles = GetTilesInRandomOrder( tiles, MT_C );
	MT_RETIF();

	for ( auto& tile : randomtiles ) {
		*tile->elevation.center += amount;
		*tile->elevation.bottom += amount;
		MT_RETIF();
	}

	for ( auto y = 0 ; y < 2 ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			if ( y == 0 ) {
				*tiles->TopRightVertexAt( x ) += amount;
			}
			*tiles->TopVertexAt( x, y ) += amount;
			MT_RETIF();
		}
	}
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tiles->At( x, y )->Update();
			MT_RETIF();
		}
	}
}

void MapGenerator::ScaleAllTilesBy( Tiles* tiles, float amount, MT_CANCELABLE ) {
	Log( "Multiplying all tiles by " + std::to_string( amount ) );
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	Tile* tile;
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tile = tiles->At( x, y );
			*tile->elevation.center *= amount;
			*tile->elevation.bottom *= amount;
			MT_RETIF();
		}
	}
	for ( auto y = 0 ; y < h ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			tiles->At( x, y )->Update();
			MT_RETIF();
		}
	}
}

const std::pair< Tile::elevation_t, Tile::elevation_t > MapGenerator::GetElevationsRange( Tiles* tiles, MT_CANCELABLE ) const {
	std::pair< Tile::elevation_t, Tile::elevation_t > result = {
		0,
		0
	};
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
			MT_RETIFV( {} );
		}
	}
	//Log( "Elevations range: min=" + std::to_string( result.first ) + " max=" + std::to_string( result.second ) );
	return result;
}

void MapGenerator::RemoveExtremeSlopes( Tiles* tiles, const Tile::elevation_t max_allowed_diff, MT_CANCELABLE ) {
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

	auto randomtiles = GetTilesInRandomOrder( tiles, MT_C );
	MT_RETIF();

	Log( "Checking/fixing extreme slopes" );

	while ( found ) {
		if ( elevation_fixby < elevation_fixby_max ) {
			elevation_fixby += elevation_fixby_change;
		}
		elevation_fixby_div += elevation_fixby_div_change;
		//Log( "Checking/fixing extreme slopes (pass " + std::to_string( ++pass ) + ")" );
		found = false;

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

			MT_RETIF();
		}

		if ( found ) {
			MT_RETIF();
			m_random->Shuffle( randomtiles );
		}

		MT_RETIF();
	}
}

void MapGenerator::NormalizeElevationRange( Tiles* tiles, MT_CANCELABLE ) {
	const auto w = tiles->GetWidth();
	const auto h = tiles->GetHeight();
	auto elevations_range = GetElevationsRange( tiles, MT_C );
	MT_RETIF();
	util::Clamper< Tile::elevation_t > converter(
		{
			{ elevations_range.first, elevations_range.second },
			{ Tile::ELEVATION_MIN,    Tile::ELEVATION_MAX }
		}
	);

	auto randomtiles = GetTilesInRandomOrder( tiles, MT_C );
	MT_RETIF();

	for ( auto& tile : randomtiles ) {
		tile->elevation_data.bottom = converter.Clamp( tile->elevation_data.bottom );
		tile->elevation_data.center = converter.Clamp( tile->elevation_data.center );
		MT_RETIF();
	}

	// convert top rows too
	for ( auto y = 0 ; y < 2 ; y++ ) {
		for ( auto x = y & 1 ; x < w ; x += 2 ) {
			if ( y == 0 ) {
				*tiles->TopRightVertexAt( x ) = converter.Clamp( *tiles->TopRightVertexAt( x ) );
			}
			*tiles->TopVertexAt( x, y ) = converter.Clamp( *tiles->TopVertexAt( x, y ) );
			MT_RETIF();
		}
	}
}

}
}
}
