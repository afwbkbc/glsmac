#pragma once

#include "ui/object/Section.h"

namespace task {
namespace mainmenu {
namespace lobby {

class Lobby;

CLASS( LobbySection, ui::object::Section )
	LobbySection( Lobby* lobby );

	Lobby* GetLobby() const;

private:
	Lobby* m_lobby = nullptr;
};

}
}
}
