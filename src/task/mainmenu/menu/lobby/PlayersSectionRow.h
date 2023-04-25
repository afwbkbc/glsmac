#pragma once

#include "ui/object/UIContainer.h"

namespace task {
namespace mainmenu {
namespace lobby {

class Lobby;
	
CLASS( PlayersSectionRow, UIContainer )

	PlayersSectionRow( Lobby* lobby );
	
private:
	Lobby* m_lobby = nullptr;

};

}
}
}

#include "Lobby.h"
