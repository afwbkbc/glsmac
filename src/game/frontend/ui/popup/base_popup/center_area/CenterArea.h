#pragma once

#include "game/frontend/ui/popup/base_popup/BPSection.h"

namespace ui::object {
class Button;
}

namespace game {
namespace frontend {

namespace base {
class Base;
}

namespace ui {
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
		::ui::object::Button* resource = nullptr;
		::ui::object::Button* support = nullptr;
		::ui::object::Button* psych = nullptr;
	} m_buttons = {};
	void ActivateButton( ::ui::object::Button* button );

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
