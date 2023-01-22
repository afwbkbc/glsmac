#pragma once

#include "../PopupMenu.h"

#include <string>

#include "ui/object/Section.h"
#include "ui/object/ChoiceList.h"

namespace game {
namespace mainmenu {

CLASS( HostJoin, PopupMenu )

	HostJoin( MainMenu* mainmenu );

	void Show();
	void Hide();

	void OnNext();
	
	const string GetChoice() const;
	void SetChoice( const string& choice );

private:
	Section* m_section = nullptr;
	ChoiceList* m_choices = nullptr;

};
	
}
}
