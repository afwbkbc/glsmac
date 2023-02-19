#pragma once

#include "Coastlines.h"

namespace game {
namespace world {
namespace map {

CLASS( Coastlines1, Coastlines )
	
	Coastlines1( Map* const map );
	~Coastlines1();
	
	void GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms );
	
private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
