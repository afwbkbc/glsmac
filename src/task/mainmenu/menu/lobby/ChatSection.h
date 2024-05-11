#pragma once

#include "LobbySection.h"

namespace ui::object {
class Input;
class TextView;
}

namespace task {
namespace mainmenu {
namespace lobby {

CLASS( ChatSection, LobbySection )

	ChatSection( Lobby* lobby );

	void Create() override;
	void Align() override;
	void Destroy() override;

	void AddMessage( const std::string& message );

private:
	::ui::object::TextView* m_log = nullptr;
	::ui::object::Input* m_input = nullptr;

};

}
}
}
