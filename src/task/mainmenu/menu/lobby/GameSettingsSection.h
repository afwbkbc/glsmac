#pragma once

#include <vector>

#include "LobbySection.h"

#include "game/Settings.h"

#include "ui/object/Label.h"
#include "ui/object/Dropdown.h"

namespace task {
namespace mainmenu {
namespace lobby {

CLASS( GameSettingsSection, LobbySection )

	GameSettingsSection( Lobby* lobby, ::game::GlobalSettings* game_settings );

	void Create();
	void Align();
	void Destroy();

	void UpdateRows();

private:

	::game::GlobalSettings* m_game_settings = nullptr;

	enum row_id_t {
		RI_DIFFICULTY_LEVEL,
		RI_TIME_CONTROLS,
		RI_SPACE_1,
		RI_GAME_TYPE,
		RI_PLANET_SIZE,
		RI_PLANET_OCEAN,
		RI_PLANET_EROSIVE,
		RI_PLANET_LIFEFORMS,
		RI_PLANET_CLOUDS,

		RI_MAX,
	};

	void CreateRow( const row_id_t row_id, const std::string& label, const size_t label_width, const size_t choices_width );
	void UpdateRow( const row_id_t row_id, const ::ui::object::ChoiceList::choices_t& choices, const std::string& default_choice );

	typedef struct {
		::ui::object::Label* label;
		::ui::object::Dropdown* choices;
	} element_row_t;
	std::vector< element_row_t > m_element_rows = {};
};

}
}
}
