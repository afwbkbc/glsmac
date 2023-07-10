#pragma once

#include <map>
#include <string>
#include <vector>

#include "ui/object/Section.h"
#include "ui/object/Label.h"

#include "game/rules/Rules.h"
#include "game/Player.h"

namespace task {
namespace mainmenu {
namespace lobby {

class PlayersSectionRow;
class Lobby;
	
CLASS( PlayersSection, Section )
	
	PlayersSection( Lobby* lobby );
	
	void Create();
	void Destroy();

	typedef std::map< size_t, ::game::Player > players_t;
	void ClearPlayers();
	void SetPlayers( const players_t& players );
	void UpdatePlayer( const size_t cid, const ::game::Player& player );
	
	Lobby* GetLobby();
	const std::vector< std::string >& GetFactionChoices();
	const std::vector< std::string >& GetDifficultyLevelChoices();
	
	void ApplyRules();
	
private:
	Lobby* m_lobby = nullptr;
	std::unordered_map< size_t, PlayersSectionRow* > m_player_rows = {};
	std::unordered_map< size_t, Label* > m_player_labels = {};

	// some caches for player rows
	struct {
		std::vector< std::string > factions = {};
		std::vector< std::string > difficulty_levels = {};
	} m_choices;

};

}
}
}

#include "PlayersSectionRow.h"
#include "Lobby.h"
