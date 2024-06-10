#include "MapObject.h"

#include "base/Assert.h"
#include "map/Map.h"
#include "map/tile/Tile.h"
#include "map/tile/TileState.h"

namespace game {

MapObject::MapObject( map::Map* map, map::tile::Tile* tile )
	: m_map( map )
	, m_tile( tile ) {
	//
}

map::tile::Tile* MapObject::GetTile() const {
	return m_tile;
}

const types::Vec3 MapObject::GetRenderCoords() const {
	ASSERT_NOLOG( m_tile, "tile not set" );
	const auto* ts = m_map->GetTileState( m_tile );
	ASSERT_NOLOG( ts, "ts not found" );
	const auto c = ts->layers[
		m_tile->is_water_tile
			? map::tile::LAYER_WATER
			: map::tile::LAYER_LAND
	].coords.center;
	return {
		c.x,
		-c.y,
		c.z
	};
}

}
