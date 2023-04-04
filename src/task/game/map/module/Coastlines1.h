#pragma once

#include "Coastlines.h"

namespace task {
namespace game {
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
