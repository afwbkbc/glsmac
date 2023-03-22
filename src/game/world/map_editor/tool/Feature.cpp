#include "Feature.h"

#include "../../World.h"

namespace game {
namespace world {
namespace map_editor {
namespace tool {

Feature::Feature( World* world, const MapEditor::tool_type_t type, const Tile::feature_t feature )
	: Tool( world, type )
	, m_feature( feature )
{
	//
}

const MapEditor::tiles_t Feature::Draw( map::Tile* tile, const MapEditor::draw_mode_t mode ) {
	if ( mode == MapEditor::DM_DEC ) {
		if ( ! ( tile->features & m_feature ) ) {
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
	
	// we need to reload surrounding tiles too because they need to blend correctly
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
	
}
}
}
}
