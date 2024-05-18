#pragma once

#include "ui/object/UIContainer.h"

#include "game/Player.h"

// TODO: remove this
#include "ui/object/Dropdown.h"

namespace game::slot {
class Slot;
}

namespace ui::object {
class Surface;
}

namespace task {
namespace mainmenu {
namespace lobby {

class PlayersSection;

CLASS( PlayersSectionRow, ui::object::UIContainer )

	PlayersSectionRow( PlayersSection* parent, const size_t slot_num, ::game::slot::Slot* slot, const std::string& class_name = "" );

	void Create() override;
	void Destroy() override;

private:
	PlayersSection* m_parent;
	const size_t m_slot_num;
	::game::slot::Slot* m_slot;

	struct {
		ui::object::Surface* ready = nullptr;
		ui::object::NumDropdown* actions = nullptr;
		ui::object::AssocDropdown* faction = nullptr;
		ui::object::NumDropdown* difficulty_level = nullptr;
	} m_elements = {};

};

}
}
}
