#pragma once

#include "../PopupMenu.h"

#include "ui/object/ChoiceList.h"

namespace game {
namespace mainmenu {

CLASS( Multiplayer, PopupMenu )
		
	Multiplayer( MainMenu* mainmenu );
	
	const string GetChoice() const;
	void SetChoice( const string& choice );
	
protected:
	virtual void Show();
	virtual void Hide();
	
	virtual void OnNext();

private:
	Panel* m_titlebar = nullptr;
	 	Label* m_titlelabel = nullptr;

	ChoiceList* m_choices = nullptr;
		
};

}
}

