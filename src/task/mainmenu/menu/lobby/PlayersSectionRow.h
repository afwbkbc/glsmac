#pragma once

#include "ui_legacy/object/UIContainer.h"

#include "game/backend/Player.h"

// TODO: remove this
#include "ui_legacy/object/Dropdown.h"

namespace game::backend::slot {
class Slot;
}

namespace ui_legacy::object {
class Surface;
}

namespace task {
namespace mainmenu {
namespace lobby {

class PlayersSection;

CLASS( PlayersSectionRow, ui_legacy::object::UIContainer )

	PlayersSectionRow( PlayersSection* parent, const size_t slot_num, game::backend::slot::Slot* slot, const std::string& class_name = "" );

	void Create() override;
	void Destroy() override;

private:
	PlayersSection* m_parent;
	const size_t m_slot_num;
	game::backend::slot::Slot* m_slot;

	struct {
		ui_legacy::object::Surface* ready = nullptr;
		ui_legacy::object::NumDropdown* actions = nullptr;
		ui_legacy::object::AssocDropdown* faction = nullptr;
		ui_legacy::object::NumDropdown* difficulty_level = nullptr;
	} m_elements = {};

};

}
}
}
