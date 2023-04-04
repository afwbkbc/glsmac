#pragma once

#include "../PopupMenu.h"

namespace task {
namespace mainmenu {

class MainMenu;
	
CLASS( Error, PopupMenu )
		
	Error( MainMenu* mainmenu, const std::string& error_text );
	
protected:
	virtual void Show();
	virtual void Hide();
	
	virtual void OnNext();

private:
	const std::string m_error_text = "";
	
	Label* m_error_label = nullptr;
};

}
}

