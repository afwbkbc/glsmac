#pragma once

#include <map>
#include <string>

#include "ui/object/Section.h"

namespace task {
namespace mainmenu {
namespace lobby {

class Lobby;

CLASS( LobbySection, Section )
	LobbySection( Lobby* lobby );

	Lobby* GetLobby() const;

private:
	Lobby* m_lobby = nullptr;
};

}
}
}

