#pragma once

#include <string>

#include "../PopupMenu.h"

#include "base/MTModule.h"

#include "ui/object/Section.h"
#include "ui/object/Label.h"
#include "ui/object/Input.h"

namespace game {
	
using namespace ui;
	
namespace mainmenu {

CLASS( Join, PopupMenu )

	Join( MainMenu* mainmenu );
	~Join();

	void Show();
	void Hide();

	void OnNext();
	
	const string GetChoice() const;
	void SetChoice( const string& choice );
	
private:
	
	Section* m_section = nullptr;
	
	Label* m_label_yourname = nullptr;
	Input* m_input_yourname = nullptr;

	Label* m_label_gameip = nullptr;
	Input* m_input_gameip = nullptr;
	
	mt_id_t m_mt_id = 0;
};
	
}
}
