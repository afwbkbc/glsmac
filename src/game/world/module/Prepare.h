#pragma once

#include "Module.h"

namespace game {
namespace world {
namespace module {

CLASS( Prepare, Module )

	Prepare( World* const map )
		: Module( map ) {}

	void GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) override;

};

}
}
}
