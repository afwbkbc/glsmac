#pragma once

#include "base/Base.h"

#include "../World.h"
#include "../Tile.h"
#include "../MapState.h"
#include "../TileState.h"

namespace game {
namespace world {
namespace module {

CLASS( Module, base::Base )

	Module( World* const map );

	virtual void GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) = 0;

protected:
	World* const m_map;

	const uint8_t RandomRotate() const;
};

}
}
}
