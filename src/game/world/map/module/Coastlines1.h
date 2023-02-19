#pragma once

#include "Module.h"

namespace game {
namespace world {
namespace map {

CLASS( Coastlines1, Module )
	
	Coastlines1( Map* const map );
	~Coastlines1();
	
	void GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms );
	
private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
