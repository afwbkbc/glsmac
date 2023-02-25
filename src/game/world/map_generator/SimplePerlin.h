#pragma once

#include "MapGenerator.h"

namespace game {
namespace world {
namespace map_generator {
		
CLASS( SimplePerlin, MapGenerator )
	
	void Generate( Tiles* tiles, size_t seed );
	
private:
	void GenerateRiver( Tiles* tiles, Tile* tile, uint8_t length, uint8_t direction );

};
	
}
}
}

