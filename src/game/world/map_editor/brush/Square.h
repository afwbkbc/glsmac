#pragma once

#include "Brush.h"

namespace game {
namespace world {
	class World;
namespace map_editor {
namespace brush {

CLASS( Square, Brush )
	
	Square( World* world, const MapEditor::brush_type_t type, const uint16_t width );
	
	const MapEditor::tiles_t Draw( map::Tile* center_tile );
	
private:
	const uint16_t m_width = 1;
	
};
	
}
}
}
}
