#include "CustomizeMapMenu.h"

#include "Erosive.h"

namespace task {
namespace mainmenu {

CustomizeMapMenu::CustomizeMapMenu(
	MainMenu *mainmenu,
	const std::string& title,
	const uint8_t preview_filename_position,
	const MenuBlock::choices_t& choices,
	const size_t default_choice
)
	: SlidingMenu( mainmenu, title, choices, default_choice )
	, m_preview_filename_position( preview_filename_position )
{
	//
}

void CustomizeMapMenu::SetPreviewVariant( const char variant ) {
	std::string preview = m_mainmenu->GetMapPreviewFilename();
	preview[ m_preview_filename_position ] = variant;
	m_mainmenu->SetCustomizeMapPreview( preview );
}

}
}
