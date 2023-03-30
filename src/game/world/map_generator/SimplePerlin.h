#pragma once

#include "MapGenerator.h"

namespace game {
namespace world {
namespace map_generator {
		
CLASS( SimplePerlin, MapGenerator )
	
	SimplePerlin( Random* random ) : MapGenerator( random ) {}
	
	void GenerateElevations( Tiles* tiles, const MapSettings& map_settings );
	void GenerateDetails( Tiles* tiles, const MapSettings& map_settings );
	
private:
	void GenerateRiver( Tiles* tiles, Tile* tile, uint8_t length, uint8_t direction, int8_t direction_diagonal );
	bool HasRiversNearby( Tile* current_tile, Tile* tile );

};
	
}
}
}
