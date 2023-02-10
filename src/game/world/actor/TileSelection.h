#pragma once

#include "Actor.h"

#include "../map/Map.h"

using namespace types;

namespace game {
namespace world {

namespace actor {

CLASS( TileSelection, Actor )
	
	TileSelection( map::Map::tile_vertices_t coords );
	
private:
	Texture* m_texture = nullptr;
	
};
	
}
}
}

