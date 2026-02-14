#pragma once

#include "Widget.h"

namespace game {
namespace frontend {

namespace widget {

CLASS( BasePreview, Widget )
public:
	BasePreview( Game* const game, ui::UI* const ui );

	void Update( ui::dom::Widget* const widget, const void* const data ) override;

protected:
	void Register( ui::dom::Widget* const widget ) override;

};

}
}
}
