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

namespace unit {
class Unit;
}

namespace widget {

CLASS( UnitPreview, Widget )
public:
	UnitPreview( const Game* const game, ui::UI* const ui );

	void Update( ui::dom::Widget* const widget, const unit::Unit* const tile );

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
