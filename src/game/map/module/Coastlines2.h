#pragma once

#include "Coastlines.h"

namespace game {
namespace map {
namespace module {

CLASS( Coastlines2, Coastlines )
	
	Coastlines2( Map* const map );
	~Coastlines2();
	
	void GenerateTile( const Tile* tile, TileState* ts, MapState* ms );
	
private:
	util::Perlin* m_perlin = nullptr;
};

}
}
}
