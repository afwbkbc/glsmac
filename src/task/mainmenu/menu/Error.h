#pragma once

#include "../PopupMenu.h"

namespace task {
namespace mainmenu {

class MainMenu;
	
CLASS( Error, PopupMenu )
		
	Error( MainMenu* mainmenu, const std::string& error_text );
	
protected:
	virtual void Show() override;
	virtual void Hide() override;
	
	virtual void OnNext() override;

private:
	const std::string m_error_text = "";
	
	Label* m_error_label = nullptr;
};

}
}

