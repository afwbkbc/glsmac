#pragma once

#include "Widget.h"

namespace game {
namespace frontend {

namespace unit {
class Unit;
}

namespace widget {

CLASS( UnitPreview, Widget )
public:
	UnitPreview( const Game* const game, ui::UI* const ui );

	void Update( ui::dom::Widget* const widget, const unit::Unit* const unit );

protected:
	void Register( ui::dom::Widget* const widget ) override;

};

}
}
}
