#pragma once

#include "base/Task.h"

#include <vector>
#include <string>

#include "ui/object/Image.h"
#include "types/Font.h"
#include "scene/actor/Actor.h"

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
	friend class MenuItem; // TODO: remove when sliding menu uses themes
	
	MenuTheme m_theme;

	ui::object::Image *m_background = nullptr;
	
	//vector<const types::Texture*> m_menu_item_textures = {};
	types::Font *m_menu_item_font = nullptr;
	
	MenuObject *m_menu_object = nullptr;
	
	vector<MenuObject*> m_menu_history = {};
	
};

} /* namespace mainmenu */
} /* namespace game */
