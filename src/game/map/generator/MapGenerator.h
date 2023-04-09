#pragma once

#include "base/Base.h"

#include "../Tiles.h"
#include "game/Settings.h"

#include "util/Random.h"

#include "base/MTModule.h"

namespace game {
namespace map {
namespace generator {
		
CLASS( MapGenerator, base::Base )
	
	// you should stay within these boundaries when generating
	// currently, any elevations will work because map will be normalized to valid range afterwards. but this may change in future
	static constexpr Tile::elevation_t MAPGEN_ELEVATION_MIN = -10000;
	static constexpr Tile::elevation_t MAPGEN_ELEVATION_MAX = 10000;
	
	// this is for normalizing map parameters (elevations etc) to required range after generation by mapgen
	// original SMAC has 0.1 inaccuracy (i.e. '50%-70% ocean' so 0.6+/-0.1)
	
	// acceptable inaccuracy will start from this and increase every try from it to prevent infinite loop
	static constexpr float INITIAL_ACCEPTABLE_INACCURACY = 0.05f;
	
	// how fast to increase INITIAL_ACCEPTABLE_INACCURACY.
	// this is needed to make it easier for find acceptable state for map
	static constexpr float ACCEPTABLE_INACCURACY_CHANGE = 1.025f;
	
	// if this is reached - it means we couldn't automatically modify map to acceptable state
	// so mapgen will be told to regenerate again from scratch
	// should be a bit larger than INITIAL_ACCEPTABLE_INACCURACY to reduce regenerations
	static constexpr float MAXIMUM_ACCEPTABLE_INACCURACY = 0.2f;
	
	// if this is reached - then we assume mapgen is incapable to generate something that can be tuned into acceptable state
	// so we give up and crash to prevent infinite loop
	static constexpr size_t MAXIMUM_REGENERATION_ATTEMPTS = 50;

	typedef	std::unordered_map< MapSettings::parameter_t, float > map_parameter_mappings_t;
	// 'select ocean coverage'
	const map_parameter_mappings_t TARGET_LAND_AMOUNTS = {
		{ MapSettings::MAP_OCEAN_LOW, 0.6f }, // '30-50% of surface'
		{ MapSettings::MAP_OCEAN_MEDIUM, 0.4f }, // '50-70% of surface'
		{ MapSettings::MAP_OCEAN_HIGH, 0.2f }  // '70-90% of surface'
	};
	
	// 'adjust erosive forces'
	const map_parameter_mappings_t TARGET_EVELATION_MULTIPLIERS = {
		{ MapSettings::MAP_EROSIVE_STRONG, 0.5f }, // 'strong'
		{ MapSettings::MAP_EROSIVE_AVERAGE, 0.75f }, // 'average'
		{ MapSettings::MAP_EROSIVE_WEAK, 1.0f }, // 'weak'
	};
	
	// 'native life forms'
	const map_parameter_mappings_t TARGET_FUNGUS_AMOUNTS = {
		{ MapSettings::MAP_LIFEFORMS_RARE, 0.25f }, // 'rare'
		{ MapSettings::MAP_LIFEFORMS_AVERAGE, 0.5f }, // 'average'
		{ MapSettings::MAP_LIFEFORMS_ABUNDANT, 0.75f }, // 'abundant'
	};
	
	// 'select cloud cover'
	const map_parameter_mappings_t TARGET_MOISTURE_AMOUNTS = {
		{ MapSettings::MAP_CLOUDS_SPARSE, 0.25f }, // 'sparse'
		{ MapSettings::MAP_CLOUDS_AVERAGE, 0.5f }, // 'average'
		{ MapSettings::MAP_CLOUDS_DENSE, 0.75f }, // 'dense'
	};
	
	MapGenerator( Random* random );
	
	void Generate( Tiles* tiles, const MapSettings& map_settings, MT_CANCELABLE );
	
	// generate ONLY elevations here
	virtual void GenerateElevations( Tiles* tiles, const MapSettings& map_settings, MT_CANCELABLE ) = 0;
	
	// generate everything EXCEPT FOR elevations here
	virtual void GenerateDetails( Tiles* tiles, const MapSettings& map_settings, MT_CANCELABLE ) = 0;
	
protected:
	
	// use this while generating for all random things
	Random * const m_random = 0;
	
	// get vector with all tiles in random order
	const std::vector< Tile* > GetTilesInRandomOrder( const Tiles* tiles, MT_CANCELABLE );
	
	// make terrain a bit smoother
	void SmoothTerrain( Tiles* tiles, MT_CANCELABLE, const bool smooth_land = true, const bool smooth_sea = true );

	// you can call it from map generator when you think you may have generated extreme slopes
	// if you don't and keep generating - they will be normalized more aggressively at the end and may make terrain more flat
	void FixExtremeSlopes( Tiles* tiles, MT_CANCELABLE );
	
private:

	// normalizing and fixing
	void SetLandAmount( Tiles* tiles, const float amount, MT_CANCELABLE );
	const float GetLandAmount( Tiles* tiles, MT_CANCELABLE, Tile::elevation_t elevation_diff = 0.0f );
	void SetFungusAmount( Tiles* tiles, const float amount, MT_CANCELABLE );
	const float GetFungusAmount( Tiles* tiles, MT_CANCELABLE );
	void SetMoistureAmount( Tiles* tiles, const float amount, MT_CANCELABLE );
	const float GetMoistureAmount( Tiles* tiles, MT_CANCELABLE );
	void FixImpossibleThings( Tiles* tiles, MT_CANCELABLE );
	
	// helpers
	void RaiseAllTilesBy( Tiles* tiles, Tile::elevation_t amount, MT_CANCELABLE );
	void ScaleAllTilesBy( Tiles* tiles, float amount, MT_CANCELABLE );
	const std::pair< Tile::elevation_t, Tile::elevation_t > GetElevationsRange( Tiles* tiles, MT_CANCELABLE ) const;
	void RemoveExtremeSlopes( Tiles* tiles, const Tile::elevation_t max_allowed_diff, MT_CANCELABLE );
	void NormalizeElevationRange( Tiles* tiles, MT_CANCELABLE );
	
};
	
}
}
}

