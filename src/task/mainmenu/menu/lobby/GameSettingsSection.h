#pragma once

#include <vector>

#include "LobbySection.h"

#include "ui_legacy/object/Types.h"

// TODO: remove this
#include "ui_legacy/object/Dropdown.h"

namespace game::backend::settings {
class GlobalSettings;
}

namespace ui_legacy::object {
class Label;
class Section;
class FileBrowser;
class Button;
}

namespace task {
namespace mainmenu {
namespace lobby {

CLASS( GameSettingsSection, LobbySection )

	GameSettingsSection( Lobby* lobby, game::backend::settings::GlobalSettings* game_settings );

	void Create() override;
	void Align() override;
	void Destroy() override;

	void Lock();
	void Unlock();

	void UpdateRows();

private:

	bool m_is_locked = false;
	const bool IsLocked() const;

	void ShowLoadMap();
	void HideLoadMap();

	game::backend::settings::GlobalSettings* m_game_settings = nullptr;

	enum row_id_t {
		RI_DIFFICULTY_LEVEL,
		RI_TIME_CONTROLS,
		RI_SPACE_1,
		RI_MAP_TYPE,
		RI_PLANET_SIZE,
		RI_PLANET_OCEAN,
		RI_PLANET_EROSIVE,
		RI_PLANET_LIFEFORMS,
		RI_PLANET_CLOUDS,

		RI_MAP_FILE,

		RI_MAX,
	};

	void CreateRow( const row_id_t row_id, const std::string& label, const size_t label_width, const size_t choices_width );

	void UpdateRow( const row_id_t row_id, const ::ui_legacy::object::num_choices_t& choices, const ::ui_legacy::object::num_choice_t default_choice );

	typedef struct {
		::ui_legacy::object::Label* label;
		::ui_legacy::object::NumDropdown* choices;
	} element_row_t;
	std::vector< element_row_t > m_element_rows = {};

	const std::vector< row_id_t > m_non_loadmap_rows = {
		RI_PLANET_SIZE,
		RI_PLANET_OCEAN,
		RI_PLANET_EROSIVE,
		RI_PLANET_LIFEFORMS,
		RI_PLANET_CLOUDS
	};
	const std::vector< row_id_t > m_loadmap_rows = { RI_MAP_FILE };

	void HideRows( const std::vector< row_id_t > row_ids );
	void ShowRows( const std::vector< row_id_t > row_ids );

	struct {
		::ui_legacy::object::Section* section = nullptr;
		::ui_legacy::object::FileBrowser* browser = nullptr;
		::ui_legacy::object::Button* button_ok = nullptr;
		::ui_legacy::object::Button* button_cancel = nullptr;
	} m_load_map;
};

}
}
}
