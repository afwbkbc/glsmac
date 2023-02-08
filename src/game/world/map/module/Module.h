#pragma once

#include "base/Base.h"

#include "../Map.h"
#include "../Tile.h"

namespace game {
namespace world {
namespace map {

CLASS( Module, base::Base )
	
	Module( Map* const map );

	virtual void GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms ) = 0;
	
protected:
	Map* const m_map;
	
	const uint8_t RandomRotate() const;
};
	
}
}
}
