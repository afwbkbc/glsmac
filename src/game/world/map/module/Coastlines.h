#pragma once

#include "Module.h"

namespace game {
namespace world {
namespace map {

CLASS( Coastlines, Module )
	
	Coastlines( Map* const map );
	~Coastlines();
	
	void GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms );
	
private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
