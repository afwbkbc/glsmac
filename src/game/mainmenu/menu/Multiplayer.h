#pragma once

#include "../PopupMenu.h"

#include "ui/object/ChoiceList.h"

namespace game {
namespace mainmenu {

class MainMenu;
	
class Multiplayer : public PopupMenu {
public:
	Multiplayer( MainMenu* mainmenu );
	
protected:
	virtual void Show();
	virtual void Hide();
	virtual void OnOkClick();

private:
	Panel* m_titlebar = nullptr;
	 	Label* m_titlelabel = nullptr;

	ChoiceList* m_choices = nullptr;
		
};

}
}

