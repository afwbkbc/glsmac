#pragma once

#include <vector>
#include <string>
#include <map>
#include <functional>

using namespace std;

namespace task {
namespace mainmenu {

class MainMenuTask;
	
class Menu {
public:
	
	typedef function<void()> choice_handler_t;
	typedef vector<pair<string, choice_handler_t>> choice_handlers_t;
	
	Menu( MainMenuTask *task, const string& title, const choice_handlers_t& choices );
	
	void Show();
	void OnItemClick( const string& choice );
	
protected:
	
	void NextMenu( Menu* menu );

	MainMenuTask *m_task;
	
private:
	const string m_title;
	const choice_handlers_t m_choices;
};
	
}
}

#include "MainMenuTask.h"
