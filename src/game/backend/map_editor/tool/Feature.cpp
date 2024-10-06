#include "Feature.h"

#include "game/backend/Game.h"

namespace game {
namespace backend {
namespace map_editor {
namespace tool {

Feature::Feature( Game* game, const tool_type_t type, const map::tile::feature_t feature )
	: Tool( game, type )
	, m_feature( feature ) {
	//
}

const tiles_t Feature::Draw( map::tile::Tile* tile, const draw_mode_t mode ) {
	if ( mode == DM_DEC ) {
		if ( !( tile->features & m_feature ) ) {
			return {}; // already unset
		}
		tile->features &= ~m_feature;
	}
	else if ( mode == DM_INC ) {
		if ( tile->features & m_feature ) {
			return {}; // already set
		}
		tile->features |= m_feature;
	}

	// some features will alter surrounding tiles, others won't
	if ( m_feature & ( map::tile::FEATURE_JUNGLE | map::tile::FEATURE_RIVER | map::tile::FEATURE_XENOFUNGUS ) ) {
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
}
