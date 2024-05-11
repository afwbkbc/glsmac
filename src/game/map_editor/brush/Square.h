#pragma once

#include <cstdint>

#include "Brush.h"

namespace game {
namespace map_editor {
namespace brush {

CLASS( Square, Brush )

	Square( Game* game, const brush_type_t type, const uint16_t width );

	const tiles_t Draw( map::tile::Tile* center_tile ) override;

private:
	const uint16_t m_width = 1;

};

}
}
}
