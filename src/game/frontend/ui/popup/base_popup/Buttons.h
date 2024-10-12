#pragma once

#include "BPSection.h"

namespace ui::object {
class Button;
}

namespace game {
namespace frontend {
namespace ui {
namespace popup {
namespace base_popup {

CLASS( Buttons, BPSection )

	Buttons( BasePopup* popup );

	void Create() override;
	void Destroy() override;

private:
	struct {
		::ui::object::Button* rename = nullptr;
		::ui::object::Button* ok = nullptr;
	} m_buttons = {};

};

}
}
}
}
}
