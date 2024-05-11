#pragma once

#include <string>

#include "base/Base.h"

namespace task {
namespace mainmenu {

class MainMenu;

CLASS( MenuObject, base::Base )
	MenuObject( MainMenu* mainmenu, const std::string& title );

	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void Align() {};
	virtual void Iterate() {};

	virtual void Close();
	virtual bool MaybeClose() { return false; }; // for handling right clicks in menu
	virtual const std::string GetChoice() const { return ""; };
	virtual void SetChoice( const std::string& choice ) {};

	void GoBack();
protected:
	friend class MenuItem;

	void NextMenu( MenuObject* menu_object );
	void MenuError( const std::string& error_text = "This feature is not available yet." );

	MainMenu* m_mainmenu = nullptr;
	const std::string m_title = "";

};

}
}
