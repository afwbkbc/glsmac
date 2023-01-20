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
	
	virtual void OnNext() {};
	
	bool IsAnimating();
	
protected:
	
	Panel* m_body = nullptr; // child classes can add elements here
	
private:
	Panel* m_frame = nullptr;
		Panel* m_titleframe = nullptr;
			Panel* m_titlebar = nullptr;
				Label* m_titlelabel = nullptr;
		Button* m_button_ok = nullptr;
		Button* m_button_cancel = nullptr;
	
	
	
};

}
}

#include "MainMenu.h"
