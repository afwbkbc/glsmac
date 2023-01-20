#include "Rules.h"

namespace game {
namespace mainmenu {

Rules::Rules( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "GAME RULES", {
	{ "PLAY WITH STANDARD RULES", [this] () -> void {
		m_mainmenu->m_settings.game_rules = game::Settings::GR_STANDARD;
		MenuError( "This feature is not available yet." );
	}},
	{ "PLAY WITH CURRENT RULES", [this] () -> void {
		m_mainmenu->m_settings.game_rules = game::Settings::GR_CURRENT;
		MenuError( "This feature is not available yet." );
	}},
	{ "CUSTOMIZE RULES", [this] () -> void {
		m_mainmenu->m_settings.game_rules = game::Settings::GR_CUSTOM;
		MenuError( "This feature is not available yet." );
	}},
}) {}

}
}
