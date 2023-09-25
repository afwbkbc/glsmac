#pragma once

#include <string>

#include "../PopupMenu.h"

#include "base/MTModule.h"

#include "ui/object/Section.h"
#include "ui/object/ChoiceList.h"

namespace task {

using namespace ui;

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

	Section* m_section = nullptr;
	ChoiceList* m_choices = nullptr;

};

}
}
