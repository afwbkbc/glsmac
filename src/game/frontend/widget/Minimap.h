#pragma once

#include "Widget.h"

#include "types/Vec2.h"

namespace types::texture {
class Texture;
}

namespace game {
namespace frontend {
namespace widget {

CLASS( Minimap, Widget )
public:

	Minimap( const Game* const game, ui::UI* const ui );

	const types::Vec2< ui::coord_t > FindLargestArea() const;
	void Update( const types::texture::Texture* minimap_texture );

protected:
	void Register( ui::dom::Widget* const widget ) override;

};

}
}
}
