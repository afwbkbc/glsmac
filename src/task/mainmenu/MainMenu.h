#pragma once

#include <vector>
#include <string>

#include "common/Task.h"

#include "resource/Types.h"

namespace game {
namespace backend {
class State;
}
}

namespace util::random {
class Random;
}

namespace ui {
namespace object {
class Label;
class Surface;
class SoundEffect;
}
namespace event {
class UIEventHandler;
}
}

namespace task {
namespace mainmenu {

class MenuObject;
class Theme;

CLASS( MainMenu, common::Task )
	void Start() override;
	void Iterate() override;
	void Stop() override;

	::game::backend::State* m_state = nullptr;

	void ShowMenu( MenuObject* menu_object );
	void GoBack();
	void MenuError( const std::string& error_text );
	void InitSinglePlayer();
	void StartGame();
	void ShowErrorOnStart( const std::string& error );

	void SetCustomizeMapPreview( const std::string& preview_filename );
	const std::string& GetMapPreviewFilename() const;
	void SetCustomizeMapMoons( const uint8_t moons_count );

	util::random::Random* GetRandom();

private:
	Theme* m_theme;

	const ::ui::event::UIEventHandler* m_key_handler = nullptr;
	const ::ui::event::UIEventHandler* m_mouse_handler = nullptr;

	::ui::object::Surface* m_background = nullptr;
	::ui::object::Surface* m_customize_map_preview = nullptr;
	::ui::object::Surface* m_customize_map_moons = nullptr;

	std::string m_customize_map_preview_filename = "";
	std::vector< std::string > m_customize_map_preview_history = {};

	MenuObject* m_menu_object = nullptr;
	std::vector< MenuObject* > m_menu_history = {};
	std::vector< std::string > m_choice_history = {};

	MenuObject* m_menu_next = nullptr;
	bool m_goback = false;

	::ui::object::SoundEffect* m_music = nullptr;

	::ui::object::Label* m_glsmac_logo = nullptr;

	util::random::Random* m_random;

	std::string m_show_error_on_start = "";

	void ResizeCustomizeMapPreview();

	const resource::resource_t GetPreviewResource( const std::string& preview_filename ) const;
	const resource::resource_t GetMoonsResource( const uint8_t moons_count ) const;

};

}
}
