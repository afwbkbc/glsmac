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
}

CLASS( BasePopup, Popup )

	BasePopup( Game* game, base::Base* base );

	void Create() override;
	void Destroy() override;

	base::Base* GetBase() const;

protected:
	void OnOpen() override;
	void OnClose() override;

private:
	base::Base* m_base = nullptr;

	bottom_bar::BottomBar* m_bottom_bar = nullptr;

};

}
}
}
}
}
