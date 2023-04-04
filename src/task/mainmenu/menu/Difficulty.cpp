#include "Difficulty.h"

#include "Rules.h"

namespace task {
namespace mainmenu {

Difficulty::Difficulty( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "PICK A DIFFICULTY LEVEL", {
	{ "CITIZEN", {
		CH( this ) {
			m_mainmenu->m_settings.global.difficulty = task::GlobalSettings::DIFFICULTY_CITIZEN;
			NEWV( menu, Rules, m_mainmenu );
			NextMenu( menu );
		}
	}},
	{ "SPECIALIST", {
		CH( this ) {
			m_mainmenu->m_settings.global.difficulty = task::GlobalSettings::DIFFICULTY_SPECIALIST;
			NEWV( menu, Rules, m_mainmenu );
			NextMenu( menu );
		}
	}},
	{ "TALENT", {
		CH( this ) {
			m_mainmenu->m_settings.global.difficulty = task::GlobalSettings::DIFFICULTY_TALENT;
			NEWV( menu, Rules, m_mainmenu );
			NextMenu( menu );
		}
	}},
	{ "LIBRARIAN", {
		CH( this ) {
			m_mainmenu->m_settings.global.difficulty = task::GlobalSettings::DIFFICULTY_LIBRARIAN;
			NEWV( menu, Rules, m_mainmenu );
			NextMenu( menu );
		}
	}},
	{ "THINKER", {
		CH( this ) {
			m_mainmenu->m_settings.global.difficulty = task::GlobalSettings::DIFFICULTY_THINKER;
			NEWV( menu, Rules, m_mainmenu );
			NextMenu( menu );
		}
	}},
	{ "TRANSCEND", {
		CH( this ) {
			m_mainmenu->m_settings.global.difficulty = task::GlobalSettings::DIFFICULTY_TRANSCEND;
			NEWV( menu, Rules, m_mainmenu );
			NextMenu( menu );
		}
	}}
}) {}

}
}
