#pragma once

#include "MenuObject.h"

#include <string>

using namespace std;

namespace game {
namespace mainmenu {
	
class MainMenu;

class PopupMenu : public MenuObject {
public:
	PopupMenu( MainMenu *mainmenu, const string& title );

	virtual void Show();
	virtual void Hide();
};

}
}

#include "MainMenu.h"
