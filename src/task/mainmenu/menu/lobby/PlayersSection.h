#pragma once

#include <vector>
#include <string>

#include "LobbySection.h"

#include "ui/object/Types.h"

namespace game::slot {
class Slot;
}

namespace task {
namespace mainmenu {
namespace lobby {

class Lobby;
class PlayersSectionRow;

CLASS( PlayersSection, LobbySection )

	PlayersSection( Lobby* lobby );

	void Create() override;
	void Align() override;
	void Destroy() override;

	void UpdateSlot( const size_t slot_num, ::game::slot::Slot* slot );
	void UpdateSlots( std::vector< ::game::slot::Slot >& slots );

	const ui::object::assoc_choices_t& GetFactionChoices();
	const ui::object::num_choices_t& GetDifficultyLevelChoices();

private:
	std::vector< PlayersSectionRow* > m_slots = {};

	// some caches for player rows
	struct {
		ui::object::assoc_choices_t factions = {};
		ui::object::num_choices_t difficulty_levels = {};
	} m_choices;

};

}
}
}
