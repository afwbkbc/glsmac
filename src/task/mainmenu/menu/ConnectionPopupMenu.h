#pragma once

#include "../PopupMenu.h"

#include "game/connection/Connection.h"

namespace task {
namespace mainmenu {

CLASS( ConnectionPopupMenu, PopupMenu )

	ConnectionPopupMenu( MainMenu* mainmenu, const std::string& title );

	void Iterate() override;

protected:
	void SetConnection( ::game::connection::Connection* connection );

};

}
}
