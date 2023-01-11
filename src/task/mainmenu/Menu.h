#pragma once

#include "ui/object/UIContainer.h"

#include <vector>
#include <string>

#include "types/Texture.h"
#include "types/Font.h"
#include "util/Timer.h"

#include "MenuItem.h"

using namespace ui::object;
using namespace std;

namespace task {
namespace mainmenu {
	
class MainMenuTask;

CHILD_CLASS(Menu, UIContainer)
	Menu( MainMenuTask* task );

	// how much pixels to shift when fully closed
	static const size_t MENU_CLOSED_POSITION = 400;
	
	// per ms
	static const size_t SLIDING_SPEED = 5;

	void Create();
	void Destroy();
	void Iterate();
	
	void AddItem(const string& text);
	
protected:
	friend class MenuItem;
	
	void OnItemClicked( const string& choice );
	
	vector<string> m_items;
	
	MainMenuTask* m_task;
	vector<MenuItem*> m_menu_items;
	
	string m_clicked_choice = "";
	
	ssize_t m_slide_value = -MENU_CLOSED_POSITION;
	ssize_t m_slide_change = SLIDING_SPEED;
	util::Timer m_slide_timer;
};

}
}

#include "MainMenuTask.h"
