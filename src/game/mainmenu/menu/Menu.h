#pragma once

#include <vector>
#include <string>
#include <map>
#include <functional>

using namespace std;

namespace game {
namespace mainmenu {

class MainMenu;
	
class Menu {
public:
	
	typedef function<void()> choice_handler_t;
	typedef vector<pair<string, choice_handler_t>> choice_handlers_t;
	
	Menu( MainMenu *mainmenu, const string& title, const choice_handlers_t& choices );
	
	void Show();
	void OnItemClick( const string& choice );
	
protected:
	
	void NextMenu( Menu* menu );

	MainMenu *m_mainmenu = nullptr;
	
private:
	const string m_title = "";
	const choice_handlers_t m_choices = {};
};
	
}
}

#include "../MainMenu.h"
