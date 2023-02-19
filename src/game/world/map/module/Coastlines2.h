#pragma once

#include "Module.h"

namespace game {
namespace world {
namespace map {

CLASS( Coastlines2, Module )
	
	Coastlines2( Map* const map );
	~Coastlines2();
	
	void GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms );
	
private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
