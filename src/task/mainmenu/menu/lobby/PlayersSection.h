#pragma once

#include <map>
#include <string>

#include "ui/object/Section.h"
#include "ui/object/Label.h"
#include "ui/object/ChoiceList.h"

#include "game/rules/Rules.h"
#include "game/Slot.h"

namespace task {
namespace mainmenu {
namespace lobby {

class PlayersSectionRow;
class Lobby;
	
CLASS( PlayersSection, Section )
	
	PlayersSection( Lobby* lobby );
	
	void Create();
	void Destroy();

	void UpdateSlot( const size_t slot_num, ::game::Slot* slot );
	
	Lobby* GetLobby();
	const ChoiceList::choices_t& GetFactionChoices();
	const ChoiceList::choices_t& GetDifficultyLevelChoices();
	
	void ApplyRules();
	
private:
	Lobby* m_lobby = nullptr;
	std::vector< PlayersSectionRow* > m_slots = {};

	// some caches for player rows
	struct {
		ChoiceList::choices_t factions = {};
		ChoiceList::choices_t difficulty_levels = {};
	} m_choices;

};

}
}
}

#include "PlayersSectionRow.h"
#include "Lobby.h"
