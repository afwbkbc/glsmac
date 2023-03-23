#pragma once

#include <vector>
#include <string>

#include "../UI.h"

#include "ui/object/Surface.h"
#include "ui/object/LabelButton.h"
#include "ui/event/UIEventHandler.h"

// return true to close menus, false otherwise
#define SMH(...) [__VA_ARGS__] ( LabelButton* button, menu_item_t item ) -> bool
	
namespace game {
namespace world {
namespace ui {

CLASS( SideMenu, UI )

	SideMenu( World* world, const uint8_t item_height = 18, const uint8_t margin = 4 );
	~SideMenu();
	
	void Create();
	void Destroy();

	void Show();
	void Hide();

protected:

	struct menu_item_t {
		const std::string label = "";
		SideMenu* submenu = nullptr;
		const std::function<bool( LabelButton* button, menu_item_t item )> on_click = 0;
	};

	// call from constructor
	void AddItem( const std::string& label, std::function<bool( LabelButton* button, menu_item_t item )> on_click = 0 );
	void AddSubMenu( const std::string& label, SideMenu* submenu );
	
private:
	const uint8_t m_item_height = 0;
	const uint8_t m_margin = 0;

	std::vector< menu_item_t > m_menu_items = {};

	Surface* m_background = nullptr;
	std::vector< LabelButton* > m_buttons = {};
	
	SideMenu* m_active_submenu = nullptr;
	LabelButton* m_active_button = nullptr;
};

}
}
}
