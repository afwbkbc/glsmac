#pragma once

#include "BBSection.h"

#include "ui/object/Button.h"

#include "Messages.h"
#include "Toolbar.h"
#include "InfoPanels.h"

namespace task {
namespace game {
namespace ui {

CLASS( MiddleArea, BBSection )

	MiddleArea( Game* game );

	void Create() override;
	void Destroy() override;

	void AddMessage( const std::string& text );
	void UpdateMapFileName();

private:

	struct {
		::ui::Button* messages = nullptr;
		::ui::Button* toolbar = nullptr;
		::ui::Button* info_panels = nullptr;
	} m_buttons;
	::ui::Button* m_active_button = nullptr;

	struct {
		Messages* messages = nullptr;
		Toolbar* toolbar = nullptr;
		InfoPanels* info_panels = nullptr;
	} m_pages;
	MiddleAreaPage* m_active_page = nullptr;

	void SwitchPage( ::ui::Button* active_button, MiddleAreaPage* active_page );
};

}
}
}
