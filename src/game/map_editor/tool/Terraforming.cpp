#include "Terraforming.h"

#include "../../Game.h"

namespace game {
namespace map_editor {
namespace tool {

Terraforming::Terraforming( Game* game, const MapEditor::tool_type_t type, const map::Tile::terraforming_t terraforming )
	: Tool( game, type )
	, m_terraforming( terraforming ) {
	//
}

const MapEditor::tiles_t Terraforming::Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) {
	if ( mode == MapEditor::DM_DEC ) {
		if ( !( tile->terraforming & m_terraforming ) ) {
			return {}; // already unset
		}
		tile->terraforming &= ~m_terraforming;
	}
	else if ( mode == MapEditor::DM_INC ) {
		if ( tile->terraforming & m_terraforming ) {
			return {}; // already set
		}
		tile->terraforming |= m_terraforming;
	}

	// some terraforming types will alter surrounding tiles, others won't
	if ( m_terraforming & ( map::Tile::T_FOREST | map::Tile::T_FARM | map::Tile::T_SOIL_ENRICHER | map::Tile::T_ROAD | map::Tile::T_MAG_TUBE ) ) {
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
