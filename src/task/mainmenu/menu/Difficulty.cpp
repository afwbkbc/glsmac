#include "Difficulty.h"

#include "Rules.h"

namespace task {
namespace mainmenu {

Difficulty::Difficulty( MainMenuTask *task ) : Menu( task, "PICK A DIFFICULTY LEVEL", {
	{ "CITIZEN", [this] () -> void {
		m_task->m_settings.difficulty = game::Settings::DIFFICULTY_CITIZEN;
		NextMenu( new Rules( m_task ) );
	}},
	{ "SPECIALIST", [this] () -> void {
		m_task->m_settings.difficulty = game::Settings::DIFFICULTY_SPECIALIST;
		NextMenu( new Rules( m_task ) );
	}},
	{ "TALENT", [this] () -> void {
		m_task->m_settings.difficulty = game::Settings::DIFFICULTY_TALENT;
		NextMenu( new Rules( m_task ) );
	}},
	{ "LIBRARIAN", [this] () -> void {
		m_task->m_settings.difficulty = game::Settings::DIFFICULTY_LIBRARIAN;
		NextMenu( new Rules( m_task ) );
	}},
	{ "THINKER", [this] () -> void {
		m_task->m_settings.difficulty = game::Settings::DIFFICULTY_THINKER;
		NextMenu( new Rules( m_task ) );
	}},
	{ "TRANSCEND", [this] () -> void {
		m_task->m_settings.difficulty = game::Settings::DIFFICULTY_TRANSCEND;
		NextMenu( new Rules( m_task ) );		
	}}
}) {}

}
}
