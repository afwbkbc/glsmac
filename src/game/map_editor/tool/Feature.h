#pragma once

#include "Tool.h"

#include "game/map/tile/Types.h"

namespace game {
namespace map_editor {
namespace tool {

CLASS( Feature, Tool )

	Feature( Game* game, const tool_type_t type, const map::tile::feature_t feature );

	const tiles_t Draw( map::tile::Tile* tile, const draw_mode_t mode ) override;

private:
	const map::tile::feature_t m_feature = map::tile::FEATURE_NONE;

};

}
}
}
