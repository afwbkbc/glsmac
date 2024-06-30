#pragma once

#include "Brush.h"

namespace game {
namespace map_editor {
namespace brush {

CLASS( Cross, Brush )

	Cross( Game* game );

	const tiles_t Draw( map::tile::Tile* center_tile ) override;

};

}
}
}
