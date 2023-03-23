#pragma once

#include "Module.h"

namespace game {
namespace world {
namespace map {

CLASS( Sprites, Module )

	Sprites( Map* const map ) : Module( map ) {}
	void GenerateTile( const Tile* tile, Map::tile_state_t* ts, Map::map_state_t* ms );
	
private:
	void GenerateSprite( const Tile* tile, Map::tile_state_t* ts, const std::string& name, const Map::pcx_texture_coordinates_t& tex_coords );
	
};
	
}
}
}
