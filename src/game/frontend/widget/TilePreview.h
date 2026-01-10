#pragma once

#include "Widget.h"

namespace scene::actor {
class Mesh;
}

namespace types::texture {
class Texture;
}

namespace game {
namespace frontend {

namespace tile {
class Tile;
}

namespace widget {

CLASS( TilePreview, Widget )
public:
	TilePreview( const Game* const game, ui::UI* const ui );

	void Update( const tile::Tile* const tile );

protected:
	void Register( ui::dom::Widget* const widget ) override;

private:
	struct preview_layer_t {
		scene::actor::Mesh* object;
		types::texture::Texture* texture;
	};
	std::vector< preview_layer_t > m_preview_layers = {}; // multiple layers of textures

};

}
}
}
