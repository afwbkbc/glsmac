#pragma once

#include "BBSection.h"

namespace ui::object {
class Button;
}

namespace task {
namespace game {
namespace ui {

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
		::ui::object::Button* messages = nullptr;
		::ui::object::Button* toolbar = nullptr;
		::ui::object::Button* info_panels = nullptr;
	} m_buttons;
	::ui::object::Button* m_active_button = nullptr;

	struct {
		Messages* messages = nullptr;
		Toolbar* toolbar = nullptr;
		InfoPanels* info_panels = nullptr;
	} m_pages;
	MiddleAreaPage* m_active_page = nullptr;

	void SwitchPage( ::ui::object::Button* active_button, MiddleAreaPage* active_page );
};

}
}
}
