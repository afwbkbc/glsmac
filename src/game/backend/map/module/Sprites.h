#pragma once

#include "Module.h"

#include "game/backend/map/Types.h"

namespace game {
namespace backend {
namespace map {
namespace module {

CLASS( Sprites, Module )

	Sprites( Map* const map )
		: Module( map ) {}
	void GenerateTile( const tile::Tile* tile, tile::TileState* ts, MapState* ms ) override;

private:
	void GenerateSprite( const tile::Tile* tile, tile::TileState* ts, const std::string& name, const pcx_texture_coordinates_t& tex_coords, const float z_index );

};

}
}
}
}
