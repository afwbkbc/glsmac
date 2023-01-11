#include "Menu.h"

using namespace std;

#include "game/Settings.h"

namespace task {
namespace mainmenu {

Menu::Menu( MainMenuTask* task, const string& title, const choice_handlers_t& choices )
	: m_task( task )
	, m_title( title )
	, m_choices ( choices )
{
	//
}

void Menu::Show() {
	vector<string> choice_texts = {};
	for (auto& it : m_choices) {
		choice_texts.push_back( it.first );
	}
	m_task->CreateMenu( m_title, choice_texts );
}

void Menu::OnItemClick( const string& choice ) {
	for (auto& it : m_choices) {
		if (it.first == choice) {
			it.second();
			return;
		}
	}
}

void Menu::NextMenu( Menu* menu ) {
	m_task->ShowMenu( menu );
}

}
}
