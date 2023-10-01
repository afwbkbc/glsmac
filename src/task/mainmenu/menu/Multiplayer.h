#pragma once

#include "../PopupMenu.h"

#include "ui/object/Section.h"
#include "ui/object/ChoiceList.h"

namespace task {
namespace mainmenu {

CLASS( Multiplayer, PopupMenu )

	Multiplayer( MainMenu* mainmenu );

	const std::string GetChoice() const override;
	void SetChoice( const std::string& choice ) override;

protected:
	virtual void Show() override;
	virtual void Hide() override;

	void OnNext() override;

private:

	Section* m_section = nullptr;
	ChoiceList* m_choices = nullptr;

};

}
}

