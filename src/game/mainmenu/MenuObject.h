#pragma once

#include <string>

#include "base/Base.h"

using namespace std;

namespace game {
namespace mainmenu {
	
class MainMenu;
	
CLASS( MenuObject, base::Base )
	MenuObject( MainMenu *mainmenu, const string& title );
	
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void Iterate() {};
	
	virtual void Close();
	virtual bool MaybeClose() { return false; }; // for handling right clicks in menu
	virtual const string GetChoice() const { return ""; };
	virtual void SetChoice( const string& choice ) {};
	
	void GoBack();
protected:
	friend class MenuItem;
	
	void NextMenu( MenuObject* menu_object );
	void MenuError( const string& error_text = "This feature is not available yet." );

	MainMenu *m_mainmenu = nullptr;
	const string m_title = "";
	
};

}
}

#include "MainMenu.h"
