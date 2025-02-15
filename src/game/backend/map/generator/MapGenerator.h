#pragma once

#include "common/Common.h"

#include "common/MTTypes.h"
#include "game/backend/map/tile/Types.h"
#include "game/backend/settings/Types.h"

namespace util::random {
class Random;
}

namespace game {
namespace backend {

class Game;

namespace settings {
class MapSettings;
}

namespace map {

namespace tile {
class Tile;
class Tiles;
}

namespace generator {

CLASS( MapGenerator, common::Class )

	// you should stay within these boundaries when generating
	// currently, any elevations will work because map will be normalized to valid range afterwards. but this may change in future
	static constexpr tile::elevation_t MAPGEN_ELEVATION_MIN = -10000;
	static constexpr tile::elevation_t MAPGEN_ELEVATION_MAX = 10000;

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

	typedef std::unordered_map< settings::map_config_value_t, float > map_config_mappings_t;
	// 'select ocean coverage'
	const map_config_mappings_t TARGET_LAND_AMOUNTS = {
		{ settings::MAP_CONFIG_OCEAN_LOW,    0.6f }, // '30-50% of surface'
		{ settings::MAP_CONFIG_OCEAN_MEDIUM, 0.4f }, // '50-70% of surface'
		{ settings::MAP_CONFIG_OCEAN_HIGH,   0.2f }  // '70-90% of surface'
	};

	// 'adjust erosive forces'
	const map_config_mappings_t TARGET_EVELATION_MULTIPLIERS = {
		{ settings::MAP_CONFIG_EROSIVE_STRONG,  0.5f }, // 'strong'
		{ settings::MAP_CONFIG_EROSIVE_AVERAGE, 0.75f }, // 'average'
		{ settings::MAP_CONFIG_EROSIVE_WEAK,    1.0f }, // 'weak'
	};

	// 'native life forms'
	const map_config_mappings_t TARGET_FUNGUS_AMOUNTS = {
		{ settings::MAP_CONFIG_LIFEFORMS_RARE,     0.25f }, // 'rare'
		{ settings::MAP_CONFIG_LIFEFORMS_AVERAGE,  0.5f }, // 'average'
		{ settings::MAP_CONFIG_LIFEFORMS_ABUNDANT, 0.75f }, // 'abundant'
	};

	// 'select cloud cover'
	const map_config_mappings_t TARGET_MOISTURE_AMOUNTS = {
		{ settings::MAP_CONFIG_CLOUDS_SPARSE,  0.25f }, // 'sparse'
		{ settings::MAP_CONFIG_CLOUDS_AVERAGE, 0.5f }, // 'average'
		{ settings::MAP_CONFIG_CLOUDS_DENSE,   0.75f }, // 'dense'
	};

	MapGenerator( Game* game, util::random::Random* random );

	void Generate( tile::Tiles* tiles, const settings::MapSettings* map_settings, MT_CANCELABLE );

	// generate ONLY elevations here
	virtual void GenerateElevations( tile::Tiles* tiles, const settings::MapSettings* map_settings, MT_CANCELABLE ) = 0;

	// generate everything EXCEPT FOR elevations here
	virtual void GenerateDetails( tile::Tiles* tiles, const settings::MapSettings* map_settings, MT_CANCELABLE ) = 0;

protected:

	Game* m_game = nullptr;

	// use this while generating for all random things
	util::random::Random* const m_random = 0;

	// get vector with all tiles in random order
	const std::vector< tile::Tile* > GetTilesInRandomOrder( tile::Tiles* tiles, MT_CANCELABLE );

	// make terrain a bit smoother
	void SmoothTerrain( tile::Tiles* tiles, MT_CANCELABLE, const bool smooth_land = true, const bool smooth_water = true );

	// you can call it from map generator when you think you may have generated extreme slopes
	// if you don't and keep generating - they will be normalized more aggressively at the end and may make terrain more flat
	void FixExtremeSlopes( tile::Tiles* tiles, MT_CANCELABLE );

private:

	// normalizing and fixing
	void SetLandAmount( tile::Tiles* tiles, const float amount, MT_CANCELABLE );
	const float GetLandAmount( tile::Tiles* tiles, MT_CANCELABLE, tile::elevation_t elevation_diff = 0.0f );
	void SetFungusAmount( tile::Tiles* tiles, const float amount, MT_CANCELABLE );
	const float GetFungusAmount( tile::Tiles* tiles, MT_CANCELABLE );
	void SetMoistureAmount( tile::Tiles* tiles, const float amount, MT_CANCELABLE );
	const float GetMoistureAmount( tile::Tiles* tiles, MT_CANCELABLE );
	void FixImpossibleThings( tile::Tiles* tiles, MT_CANCELABLE );

	// helpers
	void RaiseAllTilesBy( tile::Tiles* tiles, tile::elevation_t amount, MT_CANCELABLE );
	void ScaleAllTilesBy( tile::Tiles* tiles, float amount, MT_CANCELABLE );
	const std::pair< tile::elevation_t, tile::elevation_t > GetElevationsRange( tile::Tiles* tiles, MT_CANCELABLE ) const;
	void RemoveExtremeSlopes( tile::Tiles* tiles, const tile::elevation_t max_allowed_diff, MT_CANCELABLE );
	void NormalizeElevationRange( tile::Tiles* tiles, MT_CANCELABLE );

};

}
}
}
}
