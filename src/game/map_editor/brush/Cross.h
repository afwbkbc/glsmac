#pragma once

#include "Brush.h"

namespace game {
namespace map_editor {
namespace brush {

CLASS( Cross, Brush )

	Cross( Game* game );

	const MapEditor::tiles_t Draw( map::Tile* center_tile ) override;

};

}
}
}
