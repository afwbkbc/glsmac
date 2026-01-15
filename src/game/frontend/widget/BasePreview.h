#pragma once

#include "Widget.h"

namespace game {
namespace frontend {

namespace base {
class Base;
}

namespace widget {

CLASS( BasePreview, Widget )
public:
	BasePreview( const Game* const game, ui::UI* const ui );

	void Update( ui::dom::Widget* const widget, const base::Base* const unit );

protected:
	void Register( ui::dom::Widget* const widget ) override;

};

}
}
}
