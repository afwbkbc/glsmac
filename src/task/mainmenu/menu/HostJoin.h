#pragma once

#include "task/mainmenu/PopupMenu.h"

// TODO: remove this
#include "ui_legacy/object/ChoiceList.h"

namespace ui_legacy::object {
class Section;
}

namespace task {
namespace mainmenu {

CLASS( HostJoin, PopupMenu )

	HostJoin( MainMenu* mainmenu );
	~HostJoin();

	void Show() override;
	void Hide() override;

	void OnNext() override;

	const std::string GetChoice() const override;
	void SetChoice( const std::string& choice ) override;

private:

	ui_legacy::object::Section* m_section = nullptr;
	ui_legacy::object::NumChoiceList* m_choices = nullptr;

};

}
}
