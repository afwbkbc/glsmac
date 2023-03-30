#pragma once

#include "MapGenerator.h"

namespace game {
namespace world {
namespace map_generator {
		
CLASS( SimpleRandom, MapGenerator )
	
	SimpleRandom( Random* random ) : MapGenerator( random ) {}
	
	void GenerateElevations( Tiles* tiles, const MapSettings& map_settings );
	void GenerateDetails( Tiles* tiles, const MapSettings& map_settings );
	
};
	
}
}
}
