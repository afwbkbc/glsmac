#pragma once

#include "Brush.h"

namespace game {
namespace backend {
namespace map_editor {
namespace brush {

CLASS( Dot, Brush )

	Dot( Game* game );

	const tiles_t Draw( map::tile::Tile* center_tile ) override;

};

}
}
}
}
