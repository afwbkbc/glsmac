#pragma once

#include "Coastlines.h"

namespace task {
namespace game {
namespace map {

CLASS( Coastlines2, Coastlines )
	
	Coastlines2( Map* const map );
	~Coastlines2();
	
	void GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms );
	
private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
