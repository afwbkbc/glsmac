#pragma once

#include "base/Task.h"

#include <vector>
#include <string>

#include "ui/object/Surface.h"
#include "ui/object/SoundEffect.h"

#include "scene/actor/Sound.h"

#include "MenuObject.h"

#include "game/Settings.h"

#include "ui/Theme.h"

using namespace std;
using namespace types;

using namespace ui;
namespace ui {
using namespace object;
}

namespace game {
namespace mainmenu {

CLASS( MainMenu, base::Task )
	void Start();
	void Iterate();
	void Stop();
	
	game::Settings m_settings = {};
	
	void ShowMenu( MenuObject* menu_object );
	void GoBack();
	void MenuError( const string& error_text );
	
protected:
	Theme m_theme;

	const UIEventHandler* m_key_handler = nullptr;
	const UIEventHandler* m_mouse_handler = nullptr;
	
	Surface *m_background = nullptr;
	
	MenuObject *m_menu_object = nullptr;
	vector<MenuObject*> m_menu_history = {};
	vector<string> m_choice_history = {};
	
	MenuObject *m_menu_next = nullptr;
	bool m_goback = false;
	
	SoundEffect* m_music = nullptr;
};

} /* namespace mainmenu */
} /* namespace game */
