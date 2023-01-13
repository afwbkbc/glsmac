#pragma once

#include <string>

using namespace std;

namespace game {
namespace mainmenu {
	
class MainMenu;
	
class MenuObject {
public:
	MenuObject( MainMenu *mainmenu, const string& title );
	
	virtual void Show() = 0;
	virtual void Hide() = 0;

	void GoBack();
	
protected:
	friend class MenuItem;
	
	void NextMenu( MenuObject* menu_object );

	MainMenu *m_mainmenu = nullptr;
	const string m_title = "";
	
};

}
}

#include "MainMenu.h"
