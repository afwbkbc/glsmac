#include "Rules.h"

namespace task {
namespace mainmenu {

Rules::Rules( MainMenuTask *task ) : Menu( task, "GAME RULES", {
	{ "PLAY WITH STANDARD RULES", [this] () -> void {
		m_task->m_settings.game_rules = game::Settings::GR_STANDARD;
		// TODO
	}},
	{ "PLAY WITH CURRENT RULES", [this] () -> void {
		m_task->m_settings.game_rules = game::Settings::GR_CURRENT;
		// TODO
	}},
	{ "CUSTOMIZE RULES", [this] () -> void {
		m_task->m_settings.game_rules = game::Settings::GR_CUSTOM;
		// TODO
	}},
}) {}

}
}
