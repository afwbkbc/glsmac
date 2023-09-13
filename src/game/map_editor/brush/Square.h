#pragma once

#include <cstdint>

#include "Brush.h"

namespace game {
namespace map_editor {
namespace brush {

CLASS( Square, Brush )
	
	Square( Game* game, const MapEditor::brush_type_t type, const uint16_t width );
	
	const MapEditor::tiles_t Draw( map::Tile* center_tile ) override;
	
private:
	const uint16_t m_width = 1;
	
};
	
}
}
}
