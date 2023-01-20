#pragma once

#include "base/Base.h"

#include <string>

using namespace std;

namespace game {
namespace mainmenu {
	
class MainMenu;
	
MAJOR_CLASS( MenuObject, base::Base )
	MenuObject( MainMenu *mainmenu, const string& title );
	
	virtual void Show() = 0;
	virtual void Hide() = 0;
	
	void GoBack();
	
	virtual void Close();
	virtual const string GetChoice() const { return ""; };
	virtual void SetChoice( const string& choice ) {};
	
protected:
	friend class MenuItem;
	
	void NextMenu( MenuObject* menu_object );

	MainMenu *m_mainmenu = nullptr;
	const string m_title = "";
	
};

}
}

#include "MainMenu.h"
