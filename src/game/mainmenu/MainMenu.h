#pragma once

#include <vector>
#include <string>

#include "base/Task.h"

#include "ui/object/Surface.h"
#include "ui/object/SoundEffect.h"

#include "scene/actor/Sound.h"

#include "MenuObject.h"

#include "game/Settings.h"

#include "ui/Theme.h"
#include "util/Random.h"

using namespace types;
using namespace loader::texture;

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
	void MenuError( const std::string& error_text );
	
	util::Random* GetRandom();
	
private:
	Theme m_theme;

	const UIEventHandler* m_key_handler = nullptr;
	const UIEventHandler* m_mouse_handler = nullptr;
	
	Surface *m_background = nullptr;
	
	MenuObject *m_menu_object = nullptr;
	std::vector<MenuObject*> m_menu_history = {};
	std::vector<std::string> m_choice_history = {};
	
	MenuObject *m_menu_next = nullptr;
	bool m_goback = false;
	
	SoundEffect* m_music = nullptr;
	
	util::Random m_random;

};

} /* namespace mainmenu */
} /* namespace game */
