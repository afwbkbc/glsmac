#pragma once

#include "game/frontend/ui_legacy/bottom_bar/BBSection.h"

namespace ui_legacy::object {
class Label;
class SimpleButton;
}

namespace game {
namespace frontend {
namespace base {
class Base;
}
namespace ui_legacy {
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

	::ui_legacy::object::Label* m_label = nullptr;

	struct {
		::ui_legacy::object::SimpleButton* left = nullptr;
		::ui_legacy::object::SimpleButton* right = nullptr;
	} m_arrows = {};

private:
	friend class BottomBar;
	void Update( base::Base* base );

};

}
}
}
}
}
}
