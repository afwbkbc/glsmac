#pragma once

#include "task/mainmenu/PopupMenu.h"

// TODO: remove this
#include "ui/object/ChoiceList.h"

namespace ui::object {
class Section;
}

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

	ui::object::Section* m_section = nullptr;
	ui::object::NumChoiceList* m_choices = nullptr;

};

}
}
