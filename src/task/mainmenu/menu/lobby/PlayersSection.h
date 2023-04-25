#pragma once

#include <unordered_map>
#include <string>

#include "ui/object/Section.h"
#include "ui/object/Label.h"

namespace task {
namespace mainmenu {
namespace lobby {

class Lobby;
	
CLASS( PlayersSection, Section )
	
	PlayersSection( Lobby* lobby );
	
	void Create();
	void Destroy();

	typedef std::unordered_map< size_t, std::string > players_t;
	void ClearPlayers();
	void SetPlayers( const players_t& players );
	
private:
	Lobby* m_lobby = nullptr;
	
	std::unordered_map< size_t, Label* > m_player_labels = {};


};

}
}
}

#include "Lobby.h"