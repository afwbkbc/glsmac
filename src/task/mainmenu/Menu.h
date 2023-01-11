#pragma once

#include "ui/object/UIContainer.h"

#include <vector>
#include <string>

#include "types/Texture.h"
#include "types/Font.h"

#include "MenuItem.h"

using namespace ui::object;
using namespace std;

namespace task {
namespace mainmenu {
	
class MainMenuTask;

CHILD_CLASS(Menu, UIContainer)
	Menu( const MainMenuTask* task );

	void Create();
	void Destroy();
	
	void AddItem(const string& text);
	
protected:
	friend class MenuItem;
	
	vector<string> m_items;
	
	const MainMenuTask* m_task;

	vector<MenuItem*> m_menu_items;
	
};

}
}

#include "MainMenuTask.h"
