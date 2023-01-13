#include "Difficulty.h"

#include "Rules.h"

namespace game {
namespace mainmenu {

Difficulty::Difficulty( MainMenu *mainmenu ) : SlidingMenu( mainmenu, "PICK A DIFFICULTY LEVEL", {
	{ "CITIZEN", [this] () -> void {
		m_mainmenu->m_settings.difficulty = game::Settings::DIFFICULTY_CITIZEN;
		NextMenu( new Rules( m_mainmenu ) );
	}},
	{ "SPECIALIST", [this] () -> void {
		m_mainmenu->m_settings.difficulty = game::Settings::DIFFICULTY_SPECIALIST;
		NextMenu( new Rules( m_mainmenu ) );
	}},
	{ "TALENT", [this] () -> void {
		m_mainmenu->m_settings.difficulty = game::Settings::DIFFICULTY_TALENT;
		NextMenu( new Rules( m_mainmenu ) );
	}},
	{ "LIBRARIAN", [this] () -> void {
		m_mainmenu->m_settings.difficulty = game::Settings::DIFFICULTY_LIBRARIAN;
		NextMenu( new Rules( m_mainmenu ) );
	}},
	{ "THINKER", [this] () -> void {
		m_mainmenu->m_settings.difficulty = game::Settings::DIFFICULTY_THINKER;
		NextMenu( new Rules( m_mainmenu ) );
	}},
	{ "TRANSCEND", [this] () -> void {
		m_mainmenu->m_settings.difficulty = game::Settings::DIFFICULTY_TRANSCEND;
		NextMenu( new Rules( m_mainmenu ) );		
	}}
}) {}

}
}
