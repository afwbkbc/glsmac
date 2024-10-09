#pragma once

#include "game/frontend/ui/bottom_bar/BBSection.h"

namespace ui::object {
class Label;
}

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {

class BasePopup;

namespace bottom_bar {

CLASS( BaseTitle, BBSection )

	BaseTitle( Game* game, BasePopup* popup );

	void Create() override;
	void Destroy() override;

private:
	BasePopup* m_popup;

	::ui::object::Label* m_label = nullptr;
};

}
}
}
}
}
}
