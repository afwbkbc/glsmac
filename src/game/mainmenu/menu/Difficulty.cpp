#include "Difficulty.h"

#include "Rules.h"

namespace game {
namespace mainmenu {

Difficulty::Difficulty( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "PICK A DIFFICULTY LEVEL", {
	{ "CITIZEN", [this] () -> void {
		m_mainmenu->m_settings.global.difficulty = game::GlobalSettings::DIFFICULTY_CITIZEN;
		NEWV( menu, Rules, m_mainmenu );
		NextMenu( menu );
	}},
	{ "SPECIALIST", [this] () -> void {
		m_mainmenu->m_settings.global.difficulty = game::GlobalSettings::DIFFICULTY_SPECIALIST;
		NEWV( menu, Rules, m_mainmenu );
		NextMenu( menu );
	}},
	{ "TALENT", [this] () -> void {
		m_mainmenu->m_settings.global.difficulty = game::GlobalSettings::DIFFICULTY_TALENT;
		NEWV( menu, Rules, m_mainmenu );
		NextMenu( menu );
	}},
	{ "LIBRARIAN", [this] () -> void {
		m_mainmenu->m_settings.global.difficulty = game::GlobalSettings::DIFFICULTY_LIBRARIAN;
		NEWV( menu, Rules, m_mainmenu );
		NextMenu( menu );
	}},
	{ "THINKER", [this] () -> void {
		m_mainmenu->m_settings.global.difficulty = game::GlobalSettings::DIFFICULTY_THINKER;
		NEWV( menu, Rules, m_mainmenu );
		NextMenu( menu );
	}},
	{ "TRANSCEND", [this] () -> void {
		m_mainmenu->m_settings.global.difficulty = game::GlobalSettings::DIFFICULTY_TRANSCEND;
		NEWV( menu, Rules, m_mainmenu );
		NextMenu( menu );		
	}}
}) {}

}
}
