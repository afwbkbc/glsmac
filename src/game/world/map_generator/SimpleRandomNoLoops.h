#pragma once

// to check "pointers are slow" theory

#include "MapGenerator.h"

#ifdef MAPGEN_BENCHMARK

namespace game {
namespace world {
namespace map_generator {
		
CLASS( SimpleRandomNoLoops, MapGenerator )
	
	void Generate( Tiles* tiles, size_t seed );
	
};
	
}
}
}

#endif
