#pragma once

#include <map>
#include <string>

#include "LobbySection.h"
#include "ui/object/Label.h"
#include "ui/object/ChoiceList.h"

#include "game/Settings.h"
#include "game/rules/Rules.h"
#include "game/Slot.h"

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

	void UpdateSlot( const size_t slot_num, ::game::Slot* slot );
	void UpdateSlots( std::vector< ::game::Slot >& slots );

	const AssocChoiceList::choices_t& GetFactionChoices();
	const NumChoiceList::choices_t& GetDifficultyLevelChoices();

private:
	std::vector< PlayersSectionRow* > m_slots = {};

	// some caches for player rows
	struct {
		AssocChoiceList::choices_t factions = {};
		NumChoiceList::choices_t difficulty_levels = {};
	} m_choices;

};

}
}
}

#include "PlayersSectionRow.h"
#include "Lobby.h"