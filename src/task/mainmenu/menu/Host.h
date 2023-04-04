#pragma once

#include <string>

#include "../PopupMenu.h"

#include "base/MTModule.h"

#include "ui/object/Section.h"
#include "ui/object/Label.h"
#include "ui/object/Input.h"

namespace task {
	
using namespace ui;
	
namespace mainmenu {

CLASS( Host, PopupMenu )

	Host( MainMenu* mainmenu );
	~Host();

	void Show();
	void Hide();

	void OnNext();
	
	const std::string GetChoice() const;
	void SetChoice( const std::string& choice );
	
private:
	
	Section* m_section = nullptr;
	
	Label* m_label_yourname = nullptr;
	Input* m_input_yourname = nullptr;

	Label* m_label_gamename = nullptr;
	Input* m_input_gamename = nullptr;
	
	mt_id_t m_mt_id = 0;
	
	bool m_test = false;
};
	
}
}
