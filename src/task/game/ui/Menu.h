#pragma once

#include <vector>
#include <string>

#include "UI.h"

#include "ui/object/Surface.h"
#include "ui/object/LabelButton.h"
#include "ui/event/UIEventHandler.h"

#include "util/Scroller.h"

// return true to close menus, false otherwise
#define MH(...) [__VA_ARGS__] ( LabelButton* button, menu_item_t item ) -> bool
	
namespace task {
namespace game {
namespace ui {

CLASS( Menu, UI )

	Menu( Game* game, const uint8_t item_height = 18, const uint8_t margin = 4 );
	~Menu();
	
	void Create() override;
	void Iterate() override;
	void Destroy() override;
	void ProcessEvent( event::UIEvent* event ) override;

	void Show() override;
	void Hide() override;

protected:
	struct {
		bool use_slide_animation = false;
	} m_config = {};

	struct menu_item_t {
		const std::string label = "";
		Menu* submenu = nullptr;
		const std::function<bool( LabelButton* button, menu_item_t item )> on_click = 0;
	};

	// call from constructor
	void AddItem( const std::string& label, std::function<bool( LabelButton* button, menu_item_t item )> on_click = 0 );
	void AddSubMenu( const std::string& label, Menu* submenu );
	
private:
	const size_t CalculateHeight() const;
	
	const uint8_t m_item_height = 0;
	const uint8_t m_margin = 0;

	std::vector< menu_item_t > m_menu_items = {};

	Surface* m_background = nullptr;
	struct {
		Surface* top = nullptr;
		Surface* bottom = nullptr;
	} m_frames;
	std::vector< LabelButton* > m_buttons = {};
	
	Menu* m_active_submenu = nullptr;
	LabelButton* m_active_button = nullptr;
	
	util::Scroller< coord_t > m_slide;
	bool m_is_closing = false;
};

}
}
}
