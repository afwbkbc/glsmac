#pragma once

#include "ui/object/UIContainer.h"

#include <vector>
#include <string>

#include "types/Texture.h"
#include "types/Font.h"
#include "util/Timer.h"

#include "MenuItem.h"

using namespace std;
using namespace ui::object;
using namespace ui::event;

namespace game {
namespace mainmenu {
	
class SlidingMenu;

CHILD_CLASS(MenuBlock, UIContainer)
	MenuBlock( SlidingMenu* menu );

	// how much pixels to shift when fully closed
	static const size_t MENU_CLOSED_POSITION = 400;
	
	// per ms
	static const size_t SLIDING_SPEED = 5;

	void Create();
	void Destroy();
	void Iterate();
	
	void AddItem(const string& text);
	void AddTitle(const string& text);
	
	bool IsSliding();
	
protected:
	friend class MenuItem;
	
	void OnKeyDown( const UIEvent::event_data_t* data );
	
	void OnItemClick( const string& choice );
	
	vector<string> m_items = {};
	string m_title = "";
	
	SlidingMenu* m_menu = nullptr;
	vector<MenuItem*> m_menu_items = {};
	
	string m_clicked_choice = "";
	
	ssize_t m_slide_value = -MENU_CLOSED_POSITION;
	ssize_t m_slide_change = SLIDING_SPEED;
	util::Timer m_slide_timer;
};

}
}

#include "../../SlidingMenu.h"
