#pragma once

#include <vector>
#include <string>

#include "ui/object/UIContainer.h"

#include "ui/object/SoundEffect.h"

#include "types/Texture.h"
#include "types/Font.h"
#include "util/Timer.h"

#include "MenuItem.h"

using namespace ui::object;
using namespace ui::event;

namespace game {
namespace mainmenu {
	
class SlidingMenu;

CLASS(MenuBlock, UIContainer)
	MenuBlock( SlidingMenu* menu );

	// how much pixels to shift when fully closed
	static constexpr coord_t MENU_CLOSED_POSITION = 400;
	
	// per ms
	static constexpr size_t SLIDING_SPEED = 5;

	void Create();
	void Destroy();
	void Iterate();
	
	void AddItem(const std::string& text);
	void AddTitle(const std::string& text);
	
	const std::string GetChoice() const;
	void SetChoice( const std::string& choice );
	
	void GoBack();
	bool IsSliding();
	
protected:
	friend class MenuItem;
	
	void OnItemClick( const std::string& choice );
	
	void SetActiveItem( const size_t index );
	
	std::vector<std::string> m_items = {};
	std::string m_title = "";
	
	SlidingMenu* m_menu = nullptr;
	std::vector<MenuItem*> m_menu_items = {};
	MenuItem* m_selected_item = nullptr;
	size_t m_selected_item_index = 0;
	
	std::string m_clicked_choice = "";
	
	ssize_t m_slide_value = -MENU_CLOSED_POSITION;
	ssize_t m_slide_change = SLIDING_SPEED;
	util::Timer m_slide_timer;
	
	SoundEffect* m_sound = nullptr;
};

}
}

#include "../../SlidingMenu.h"
