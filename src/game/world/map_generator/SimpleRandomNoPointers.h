#pragma once

// to check "pointers are slow" theory

#include "MapGenerator.h"

#ifdef MAPGEN_BENCHMARK

namespace game {
namespace world {
namespace map_generator {
		
CLASS( SimpleRandomNoPointers, MapGenerator )
	
	void Generate( Tiles* tiles, size_t seed );
	
};
	
}
}
}

#endif
