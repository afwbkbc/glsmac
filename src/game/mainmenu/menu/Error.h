#pragma once

#include "../PopupMenu.h"

namespace game {
namespace mainmenu {

class MainMenu;
	
CLASS( Error, PopupMenu )
		
	Error( MainMenu* mainmenu, const string& error_text );
	
protected:
	virtual void Show();
	virtual void Hide();
	
	virtual void OnNext();

private:
	const string m_error_text = "";
	
	Label* m_error_label = nullptr;
};

}
}

