#pragma once

#include "MapGenerator.h"

namespace game {
namespace map {
namespace generator {

CLASS( SimplePerlin, MapGenerator )

	SimplePerlin( Random* random )
		: MapGenerator( random ) {}

	void GenerateElevations( Tiles* tiles, const MapSettings& map_settings, MT_CANCELABLE ) override;
	void GenerateDetails( Tiles* tiles, const MapSettings& map_settings, MT_CANCELABLE ) override;

private:
	void GenerateRiver( Tiles* tiles, Tile* tile, uint8_t length, uint8_t direction, int8_t direction_diagonal, MT_CANCELABLE );
	bool HasRiversNearby( Tile* current_tile, Tile* tile );

};

}
}
}
