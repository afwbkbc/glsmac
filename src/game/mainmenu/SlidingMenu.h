#pragma once

#include <vector>
#include <string>
#include <map>
#include <functional>

#include "MenuObject.h"

#include "ui/slidingmenu/MenuBlock.h"

namespace game {
namespace mainmenu {

CLASS( SlidingMenu, MenuObject )
	
	typedef std::function<void()> choice_handler_t;
	typedef std::vector< std::pair< std::string, choice_handler_t > > choice_handlers_t;
	
	SlidingMenu( MainMenu *mainmenu, const std::string& title, const choice_handlers_t& choices );
	
	void Show();
	void Hide();
	void OnItemClick( const std::string& choice );
	const std::string GetChoice() const;
	void SetChoice( const std::string& choice );
	
	void Close();
	bool MaybeClose();

private:
	const choice_handlers_t m_choices = {};
	std::string m_choice = "";

	MenuBlock* m_menu_block = nullptr;
	
};
	
}
}

