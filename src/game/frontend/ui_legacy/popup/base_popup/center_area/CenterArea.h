#pragma once

#include "game/frontend/ui_legacy/popup/base_popup/BPSection.h"

namespace ui_legacy::object {
class Button;
}

namespace game {
namespace frontend {

namespace base {
class Base;
}

namespace ui_legacy {
namespace popup {
namespace base_popup {
namespace center_area {

class CenterAreaPage;

class Resource;
class Support;
class Psych;

CLASS( CenterArea, BPSection )

	CenterArea( BasePopup* popup );

	void Create() override;
	void Destroy() override;

private:
	friend class CenterAreaPage;
	BasePopup* GetPopup() const;

	struct {
		Resource* resource = nullptr;
		Support* support = nullptr;
		Psych* psych = nullptr;
	} m_pages = {};
	struct {
		::ui_legacy::object::Button* resource = nullptr;
		::ui_legacy::object::Button* support = nullptr;
		::ui_legacy::object::Button* psych = nullptr;
	} m_buttons = {};
	void ActivateButton( ::ui_legacy::object::Button* button );

private:
	friend class base_popup::BasePopup;
	void Update( base::Base* base );

};

}
}
}
}
}
}
