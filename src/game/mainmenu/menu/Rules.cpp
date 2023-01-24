#include "Rules.h"

namespace game {
namespace mainmenu {

Rules::Rules( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "GAME RULES", {
	{ "PLAY WITH STANDARD RULES", [this] () -> void {
		m_mainmenu->m_settings.global.game_rules = game::GlobalSettings::GR_STANDARD;
		MenuError();
	}},
	{ "PLAY WITH CURRENT RULES", [this] () -> void {
		m_mainmenu->m_settings.global.game_rules = game::GlobalSettings::GR_CURRENT;
		MenuError();
	}},
	{ "CUSTOMIZE RULES", [this] () -> void {
		m_mainmenu->m_settings.global.game_rules = game::GlobalSettings::GR_CUSTOM;
		MenuError();
	}},
}) {}

}
}
