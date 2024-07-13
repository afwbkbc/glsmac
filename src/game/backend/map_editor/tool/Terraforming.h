#pragma once

#include "Tool.h"

#include "game/backend/map/tile/Types.h"

namespace game {
namespace backend {
namespace map_editor {
namespace tool {

CLASS( Terraforming, Tool )

	Terraforming( Game* game, const tool_type_t type, const map::tile::terraforming_t terraforming );

	const tiles_t Draw( map::tile::Tile* tile, const draw_mode_t mode ) override;

private:
	const map::tile::terraforming_t m_terraforming = map::tile::TERRAFORMING_NONE;

};

}
}
}
}
