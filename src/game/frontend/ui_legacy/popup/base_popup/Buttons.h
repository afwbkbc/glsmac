#pragma once

#include "BPSection.h"

namespace ui_legacy::object {
class Button;
}

namespace game {
namespace frontend {
namespace ui_legacy {
namespace popup {
namespace base_popup {

CLASS( Buttons, BPSection )

	Buttons( BasePopup* popup );

	void Create() override;
	void Destroy() override;

private:
	struct {
		::ui_legacy::object::Button* rename = nullptr;
		::ui_legacy::object::Button* ok = nullptr;
	} m_buttons = {};

};

}
}
}
}
}
