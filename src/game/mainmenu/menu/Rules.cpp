#include "Rules.h"

namespace game {
namespace mainmenu {

Rules::Rules( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "GAME RULES", {
	{ "PLAY WITH STANDARD RULES", [this] () -> void {
		m_mainmenu->m_settings.game_rules = game::Settings::GR_STANDARD;
		// TODO
	}},
	{ "PLAY WITH CURRENT RULES", [this] () -> void {
		m_mainmenu->m_settings.game_rules = game::Settings::GR_CURRENT;
		// TODO
	}},
	{ "CUSTOMIZE RULES", [this] () -> void {
		m_mainmenu->m_settings.game_rules = game::Settings::GR_CUSTOM;
		// TODO
	}},
}) {}

}
}
