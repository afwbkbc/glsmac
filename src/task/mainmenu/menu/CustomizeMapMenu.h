#pragma once

#include <cstdint>

#include "task/mainmenu/SlidingMenu.h"

namespace task {
namespace mainmenu {

CLASS( CustomizeMapMenu, SlidingMenu )

	CustomizeMapMenu(
		MainMenu* mainmenu,
		const std::string& title,
		const uint8_t preview_filename_position,
		const choices_t& choices,
		const size_t default_choice = 0
	);

protected:
	void SetPreviewVariant( const char variant );

private:

	const uint8_t m_preview_filename_position = 0;

};

}
}
