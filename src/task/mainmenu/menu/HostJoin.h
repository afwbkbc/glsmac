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

	void Show();
	void Hide();

	void OnNext();
	
	const std::string GetChoice() const;
	void SetChoice( const std::string& choice );

private:
	
	Section* m_section = nullptr;
	ChoiceList* m_choices = nullptr;

	mt_id_t m_mt_id = 0;
};
	
}
}
