#pragma once

#include "MapGenerator.h"

namespace task {
namespace game {
namespace map_generator {
		
CLASS( SimpleRandom, MapGenerator )
	
	SimpleRandom( Random* random ) : MapGenerator( random ) {}
	
	void GenerateElevations( Tiles* tiles, const MapSettings& map_settings );
	void GenerateDetails( Tiles* tiles, const MapSettings& map_settings );
	
};
	
}
}
}
