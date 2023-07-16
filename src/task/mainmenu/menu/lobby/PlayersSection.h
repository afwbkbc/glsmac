#pragma once

#include <map>
#include <string>
#include <vector>

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

	//typedef std::map< size_t, ::game::Slot > slots_t;
	
	//void ClearSlots();
	//void SetSlots( const size_t count );
	void UpdateSlots( const std::vector< ::game::Slot >& slots );
	/*
	void ClearPlayers();
	void SetPlayers( const players_t& players );
	void UpdatePlayer( const size_t cid, const ::game::Player& player );
	*/
	Lobby* GetLobby();
	const ChoiceList::choices_t& GetFactionChoices();
	const ChoiceList::choices_t& GetDifficultyLevelChoices();
	
	void ApplyRules();
	
private:
	Lobby* m_lobby = nullptr;
	std::vector< PlayersSectionRow* > m_player_rows = {};

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
