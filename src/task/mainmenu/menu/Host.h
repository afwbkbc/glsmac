#pragma once

#include "ConnectionPopupMenu.h"

namespace ui_legacy::object {
class Section;
class Label;
class Input;
}

namespace task {
namespace mainmenu {

CLASS( Host, ConnectionPopupMenu )

	Host( MainMenu* mainmenu );
	~Host();

	void Show() override;
	void Hide() override;

	void OnNext() override;

	const std::string GetChoice() const override;
	void SetChoice( const std::string& choice ) override;

private:

	ui_legacy::object::Section* m_section = nullptr;

	ui_legacy::object::Label* m_label_yourname = nullptr;
	ui_legacy::object::Input* m_input_yourname = nullptr;

	ui_legacy::object::Label* m_label_gamename = nullptr;
	ui_legacy::object::Input* m_input_gamename = nullptr;

};

}
}
