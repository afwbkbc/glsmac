#pragma once

#include "base/Base.h"

#include "../Map.h"
#include "../Tile.h"
#include "../MapState.h"
#include "../TileState.h"

namespace game {
namespace map {
namespace module {

CLASS( Module, base::Base )
	
	Module( Map* const map );

	virtual void GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) = 0;
	
protected:
	Map* const m_map;
	
	const uint8_t RandomRotate() const;
};
	
}
}
}
