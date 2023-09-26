#pragma once

#include <vector>
#include <string>

#include "base/Task.h"

#include "ui/object/Surface.h"
#include "ui/object/SoundEffect.h"
#include "ui/object/Label.h"

#include "scene/actor/Sound.h"

#include "MenuObject.h"

#include "game/State.h"

#include "ui/Theme.h"
#include "util/Random.h"

using namespace types;
using namespace loader::texture;

using namespace ui;
namespace ui {
using namespace object;
}

namespace task {
namespace mainmenu {

CLASS( MainMenu, base::Task )
	void Start() override;
	void Iterate() override;
	void Stop() override;

	::game::State* m_state = nullptr;

	void ShowMenu( MenuObject* menu_object );
	void GoBack();
	void MenuError( const std::string& error_text );
	void StartGame();

	void SetCustomizeMapPreview( const std::string& preview_filename );
	const std::string& GetMapPreviewFilename() const;
	void SetCustomizeMapMoons( const std::string& moons_filename );

	util::Random* GetRandom();

private:
	Theme m_theme;

	const UIEventHandler* m_key_handler = nullptr;
	const UIEventHandler* m_mouse_handler = nullptr;

	Surface* m_background = nullptr;
	Surface* m_customize_map_preview = nullptr;
	Surface* m_customize_map_moons = nullptr;

	std::string m_customize_map_preview_filename = "";
	std::vector< std::string > m_customize_map_preview_history = {};

	std::string m_customize_map_moons_filename = "";

	MenuObject* m_menu_object = nullptr;
	std::vector< MenuObject* > m_menu_history = {};
	std::vector< std::string > m_choice_history = {};

	MenuObject* m_menu_next = nullptr;
	bool m_goback = false;

	SoundEffect* m_music = nullptr;

	Label* m_glsmac_logo = nullptr;

	util::Random m_random;

	void ResizeCustomizeMapPreview();

};

} /* namespace mainmenu */
} /* namespace game */
