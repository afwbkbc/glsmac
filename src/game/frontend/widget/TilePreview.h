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

namespace widget {

CLASS( TilePreview, Widget )
public:
	TilePreview( Game* const game, ui::UI* const ui );

	void Update( ui::dom::Widget* const widget, const void* const data ) override;

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
