#pragma once

#include "MapGenerator.h"

namespace game {
namespace world {
namespace map_generator {
		
CLASS( SimpleRandom, MapGenerator )
	
	void Generate( Tiles* tiles );
	
};
	
}
}
}

