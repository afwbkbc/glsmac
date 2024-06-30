#pragma once

#include "task/mainmenu/PopupMenu.h"

namespace game::connection {
class Connection;
}

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
