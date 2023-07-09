#pragma once

#include "ui/object/UIContainer.h"
#include "ui/object/Dropdown.h"

#include "game/Player.h"

using namespace ui::object;

namespace task {
namespace mainmenu {
namespace lobby {

class PlayersSection;
	
CLASS( PlayersSectionRow, UIContainer )

	PlayersSectionRow( PlayersSection* parent, const ::game::Player& player, const std::string& class_name = "" );

	void Create();
	void Destroy();

private:
	PlayersSection* m_parent;
	const ::game::Player m_player;

	struct {
		Dropdown* actions;
		Dropdown* faction;
		Dropdown* difficulty_level;
	} m_elements;
	
};

}
}
}

#include "PlayersSection.h"
