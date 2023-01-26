#pragma once

#include "base/Base.h"

#include "../map/Tiles.h"

namespace game {
namespace world {
	
using namespace map;

namespace map_generator {
		
CLASS( MapGenerator, base::Base )
	
	virtual void Generate( Tiles* tiles ) = 0;
	
};
	
}
}
}

