#pragma once

#include "Widget.h"

namespace game {
namespace frontend {

namespace tile {
class Tile;
}

namespace widget {

CLASS( TilePreview, Widget )
public:
	TilePreview( ui::UI* const ui );

	void Update( const tile::Tile* const tile );

protected:
	void Register( ui::dom::Widget* const widget ) override;

};

}
}
}
