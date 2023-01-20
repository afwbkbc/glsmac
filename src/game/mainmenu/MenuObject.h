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
	
	virtual void Close();
	virtual void MaybeClose() {}; // for handling right clicks in menu
	virtual const string GetChoice() const { return ""; };
	virtual void SetChoice( const string& choice ) {};
	
	void GoBack();
protected:
	friend class MenuItem;
	
	void NextMenu( MenuObject* menu_object );
	void MenuError( const string& error_text );

	MainMenu *m_mainmenu = nullptr;
	const string m_title = "";
	
};

}
}

#include "MainMenu.h"
