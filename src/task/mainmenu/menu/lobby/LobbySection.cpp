#include "LobbySection.h"

namespace task {
namespace mainmenu {
namespace lobby {

LobbySection::LobbySection( Lobby *lobby )
	: Section( "PopupSection" )
	, m_lobby( lobby )
{
	//
}

Lobby* LobbySection::GetLobby() const {
	return m_lobby;
}

}
}
}
