#pragma once

#include "Brush.h"

namespace game {
namespace map_editor {
namespace brush {

CLASS( Dot, Brush )

	Dot( Game* game );

	const MapEditor::tiles_t Draw( world::Tile* center_tile ) override;

};

}
}
}
