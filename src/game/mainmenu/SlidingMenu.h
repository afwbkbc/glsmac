#pragma once

#include "MenuObject.h"

#include <vector>
#include <string>
#include <map>
#include <functional>

#include "ui/slidingmenu/MenuBlock.h"

using namespace std;

namespace game {
namespace mainmenu {

class SlidingMenu : public MenuObject {
public:
	
	typedef function<void()> choice_handler_t;
	typedef vector<pair<string, choice_handler_t>> choice_handlers_t;
	
	SlidingMenu( MainMenu *mainmenu, const string& title, const choice_handlers_t& choices );
	
	void Show();
	void Hide();
	void OnItemClick( const string& choice );
	const string GetChoice() const;
	void SetChoice( const string& choice );
	
	void Close();

private:
	const choice_handlers_t m_choices = {};
	string m_choice = "";

	MenuBlock* m_menu_block = nullptr;
	
};
	
}
}

