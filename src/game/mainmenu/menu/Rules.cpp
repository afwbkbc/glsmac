#include "Rules.h"

namespace game {
namespace mainmenu {

Rules::Rules( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "GAME RULES", {
	{ "PLAY WITH STANDARD RULES", {
		CH( this ) {
			m_mainmenu->m_settings.global.game_rules = game::GlobalSettings::GR_STANDARD;
			// TODO: faction select
			m_mainmenu->StartGame();
		}
	}},
	{ "PLAY WITH CURRENT RULES", {
		CH( this ) {
			m_mainmenu->m_settings.global.game_rules = game::GlobalSettings::GR_CURRENT;
			MenuError();
		}
	}},
	{ "CUSTOMIZE RULES", {
		CH( this ) {
			m_mainmenu->m_settings.global.game_rules = game::GlobalSettings::GR_CUSTOM;
			MenuError();
		}
	}},
}) {}

}
}
