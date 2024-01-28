#pragma once

#include "Module.h"

namespace game {
namespace world {
namespace module {

CLASS( Sprites, Module )

	Sprites( World* const map )
		: Module( map ) {}
	void GenerateTile( const Tile* tile, TileState* ts, MapState* ms ) override;

private:
	void GenerateSprite( const Tile* tile, TileState* ts, const std::string& name, const Consts::pcx_texture_coordinates_t& tex_coords, const float z_index );

};

}
}
}
