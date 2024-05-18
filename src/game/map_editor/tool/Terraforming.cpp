#include "Terraforming.h"

#include "game/map/tile/Tile.h"

namespace game {
namespace map_editor {
namespace tool {

Terraforming::Terraforming( Game* game, const tool_type_t type, const map::tile::terraforming_t terraforming )
	: Tool( game, type )
	, m_terraforming( terraforming ) {
	//
}

const tiles_t Terraforming::Draw( map::tile::Tile* tile, const draw_mode_t mode ) {
	if ( mode == DM_DEC ) {
		if ( !( tile->terraforming & m_terraforming ) ) {
			return {}; // already unset
		}
		tile->terraforming &= ~m_terraforming;
	}
	else if ( mode == DM_INC ) {
		if ( tile->terraforming & m_terraforming ) {
			return {}; // already set
		}
		tile->terraforming |= m_terraforming;
	}

	// some terraforming types will alter surrounding tiles, others won't
	if ( m_terraforming & ( map::tile::TERRAFORMING_FOREST | map::tile::TERRAFORMING_FARM | map::tile::TERRAFORMING_SOIL_ENRICHER | map::tile::TERRAFORMING_ROAD | map::tile::TERRAFORMING_MAG_TUBE ) ) {
		return {
			tile,
			tile->W,
			tile->NW,
			tile->N,
			tile->NE,
			tile->E,
			tile->SE,
			tile->S,
			tile->SW,
		};
	}
	else {
		return { tile };
	}
}

}
}
}
