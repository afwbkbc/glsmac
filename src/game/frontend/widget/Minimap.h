#pragma once

#include "Widget.h"

#include "types/Vec2.h"

namespace game {
namespace frontend {
namespace widget {

CLASS( Minimap, Widget )
public:

	Minimap( Game* const game, ui::UI* const ui );

	const types::Vec2< ui::coord_t > FindLargestArea() const;

	void Update( ui::dom::Widget* const widget, const void* const data ) override;

protected:
	void Register( ui::dom::Widget* const widget ) override;

};

}
}
}
