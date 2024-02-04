#include "Feature.h"

#include "../../Game.h"

namespace game {
namespace map_editor {
namespace tool {

Feature::Feature( Game* game, const MapEditor::tool_type_t type, const map::Tile::feature_t feature )
	: Tool( game, type )
	, m_feature( feature ) {
	//
}

const MapEditor::tiles_t Feature::Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) {
	if ( mode == MapEditor::DM_DEC ) {
		if ( !( tile->features & m_feature ) ) {
			return {}; // already unset
		}
		tile->features &= ~m_feature;
	}
	else if ( mode == MapEditor::DM_INC ) {
		if ( tile->features & m_feature ) {
			return {}; // already set
		}
		tile->features |= m_feature;
	}

	// some features will alter surrounding tiles, others won't
	if ( m_feature & ( map::Tile::F_JUNGLE | map::Tile::F_RIVER | map::Tile::F_XENOFUNGUS ) ) {
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
