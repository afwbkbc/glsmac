#pragma once

#include "BBSection.h"

namespace ui_legacy::object {
class Button;
}

namespace game {
namespace frontend {
namespace ui_legacy {

class Messages;
class Toolbar;
class InfoPanels;
class MiddleAreaPage;

CLASS( MiddleArea, BBSection )

	MiddleArea( Game* game );

	void Create() override;
	void Destroy() override;

	void AddMessage( const std::string& text );
	void UpdateMapFileName();

private:

	struct {
		::ui_legacy::object::Button* messages = nullptr;
		::ui_legacy::object::Button* toolbar = nullptr;
		::ui_legacy::object::Button* info_panels = nullptr;
	} m_buttons;
	::ui_legacy::object::Button* m_active_button = nullptr;

	struct {
		Messages* messages = nullptr;
		Toolbar* toolbar = nullptr;
		InfoPanels* info_panels = nullptr;
	} m_pages;
	MiddleAreaPage* m_active_page = nullptr;

	void SwitchPage( ::ui_legacy::object::Button* active_button, MiddleAreaPage* active_page );
};

}
}
}
