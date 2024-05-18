#pragma once

#include "ConnectionPopupMenu.h"

namespace ui::object {
class Section;
class Label;
class Input;
}

namespace task {
namespace mainmenu {

CLASS( Join, ConnectionPopupMenu )

	Join( MainMenu* mainmenu );
	~Join();

	void Show() override;
	void Hide() override;

	void OnNext() override;

	const std::string GetChoice() const override;
	void SetChoice( const std::string& choice ) override;

private:

	ui::object::Section* m_section = nullptr;

	ui::object::Label* m_label_yourname = nullptr;
	ui::object::Input* m_input_yourname = nullptr;

	ui::object::Label* m_label_gameip = nullptr;
	ui::object::Input* m_input_gameip = nullptr;

};

}
}
