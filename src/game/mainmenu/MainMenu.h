#pragma once

#include "base/Task.h"

#include <vector>
#include <string>

#include "ui/object/Image.h"

#include "MenuObject.h"

#include "game/Settings.h"

#include "ui/MenuTheme.h"

using namespace std;
using namespace types;

namespace game {
namespace mainmenu {

CHILD_CLASS( MainMenu, base::Task )
	void Start();
	void Stop();
	
	game::Settings m_settings = {};
	
	void ShowMenu( MenuObject* menu_object );
	void GoBack();
	
protected:
	MenuTheme m_theme;

	ui::object::Image *m_background = nullptr;
	
	MenuObject *m_menu_object = nullptr;
	vector<MenuObject*> m_menu_history = {};
	
};

} /* namespace mainmenu */
} /* namespace game */
