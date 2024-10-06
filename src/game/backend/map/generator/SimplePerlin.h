#pragma once

#include "MapGenerator.h"

namespace game {
namespace backend {
namespace map {
namespace generator {

CLASS( SimplePerlin, MapGenerator )

	SimplePerlin( util::random::Random* random )
		: MapGenerator( random ) {}

	void GenerateElevations( tile::Tiles* tiles, const settings::MapSettings* map_settings, MT_CANCELABLE ) override;
	void GenerateDetails( tile::Tiles* tiles, const settings::MapSettings* map_settings, MT_CANCELABLE ) override;

private:
	void GenerateRiver( tile::Tiles* tiles, tile::Tile* tile, uint8_t length, uint8_t direction, int8_t direction_diagonal, MT_CANCELABLE );
	bool HasRiversNearby( tile::Tile* current_tile, tile::Tile* tile );

};

}
}
}
}
