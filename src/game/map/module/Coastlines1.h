#pragma once

#include "Coastlines.h"

namespace game {
namespace map {
namespace module {

CLASS( Coastlines1, Coastlines )
	
	Coastlines1( Map* const map );
	~Coastlines1();
	
	void GenerateTile( const Tile* tile, TileState* ts, MapState* ms );
	
private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
