#pragma once

#include "base/Base.h"

#include "../map/Tiles.h"

namespace game {
namespace world {
	
using namespace map;

namespace map_generator {
		
CLASS( MapGenerator, base::Base )
	
	// you should stay within these boundaries when generating
	// currently, any elevations will work because map will be normalized to valid range afterwards. but this may change in future
	static constexpr Tile::elevation_t MAPGEN_ELEVATION_MIN = -10000;
	static constexpr Tile::elevation_t MAPGEN_ELEVATION_MAX = 10000;

	virtual void Generate( Tiles* tiles, size_t seed ) = 0;
	
protected:
	
	// various tools
	void SmoothTerrain( Tiles* tiles, const bool smooth_land = true, const bool smooth_sea = true );
	
	void Finalize( Tiles* tiles );

};
	
}
}
}

