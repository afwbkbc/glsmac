#pragma once

#include "MenuObject.h"

#include <string>

#include "ui/object/Panel.h"
#include "ui/object/Button.h"

using namespace std;

using namespace ui;
namespace ui {
	using namespace object;
}

namespace game {
namespace mainmenu {
	
class MainMenu;

class PopupMenu : public MenuObject {
public:
	PopupMenu( MainMenu *mainmenu, const string& title );

	virtual void Show();
	virtual void Hide();
	
protected:
	Panel* m_frame = nullptr;
	Button* m_button_ok = nullptr;
	Button* m_button_cancel = nullptr;
	
	Surface *m_titlebar = nullptr;
	Surface *m_body = nullptr;
	
	
	
};

}
}

#include "MainMenu.h"
