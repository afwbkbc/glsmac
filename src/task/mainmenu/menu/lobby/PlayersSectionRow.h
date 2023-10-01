#pragma once

#include "ui/object/Surface.h"
#include "ui/object/UIContainer.h"
#include "ui/object/Dropdown.h"

#include "game/Slot.h"
#include "game/Player.h"

using namespace ui::object;

namespace task {
namespace mainmenu {
namespace lobby {

class PlayersSection;

CLASS( PlayersSectionRow, UIContainer )

	PlayersSectionRow( PlayersSection* parent, const size_t slot_num, ::game::Slot* slot, const std::string& class_name = "" );

	void Create() override;
	void Destroy() override;

private:
	PlayersSection* m_parent;
	const size_t m_slot_num;
	::game::Slot* m_slot;

	struct {
		Surface* ready = nullptr;
		Dropdown* actions = nullptr;
		Dropdown* faction = nullptr;
		Dropdown* difficulty_level = nullptr;
	} m_elements = {};

};

}
}
}

#include "PlayersSection.h"
