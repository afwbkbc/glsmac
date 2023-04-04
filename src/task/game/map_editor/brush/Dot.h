#pragma once

#include "Brush.h"

namespace task {
namespace game {
namespace map_editor {
namespace brush {

CLASS( Dot, Brush )
	
	Dot( Game* game );
	
	const MapEditor::tiles_t Draw( map::Tile* center_tile );
	
};
	
}
}
}
}
