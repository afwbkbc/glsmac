#pragma once

#include "game/frontend/ui/popup/Popup.h"

namespace ui::object {
class Section;
class Label;
class Input;
}

namespace game {
namespace frontend {

namespace base {
class Base;
}

namespace ui {
namespace popup {
namespace base_popup {

namespace bottom_bar {
class BottomBar;
class BaseTitle;
}

class Governor;
class Nutrients;
class Commerce;
class GlobalInfo;
class CenterArea;
class Yields;
class Energy;
class Facilities;
class Buttons;

CLASS( BasePopup, Popup )

	BasePopup( Game* game, base::Base* base );

	void Create() override;
	void Destroy() override;

	Game* GetGame() const;
	base::Base* GetBase() const;

protected:
	void OnOpen() override;
	void OnClose() override;

private:
	base::Base* m_base = nullptr;

	bottom_bar::BottomBar* m_bottom_bar = nullptr;
	struct {
		Governor* governor = nullptr;
		Nutrients* nutrients = nullptr;
		Commerce* commerce = nullptr;
		GlobalInfo* global_info = nullptr;
		CenterArea* center_area = nullptr;
		Yields* yields = nullptr;
		Energy* energy = nullptr;
		Facilities* facilities = nullptr;
		Buttons* buttons = nullptr;
	} m_sections = {};

	void Update( base::Base* base );

private:
	friend class bottom_bar::BaseTitle;
	void SelectNextBase();
	void SelectPrevBase();

};

}
}
}
}
}
