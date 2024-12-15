#pragma once

#include <string>

#include "MenuObject.h"

#include "ui_legacy/slidingmenu/Types.h"

namespace task {
namespace mainmenu {

class MenuBlock;

#define CH( ... ) [ __VA_ARGS__ ] () -> void

CLASS( SlidingMenu, MenuObject )

	SlidingMenu( MainMenu* mainmenu, const std::string& title, const choices_t& choices, const size_t default_choice = 0 );

	void Show() override;
	void Hide() override;
	void OnItemClick( const std::string& choice );
	const std::string GetChoice() const override;
	void SetChoice( const std::string& choice ) override;

	virtual void Close() override;
	bool MaybeClose() override;

protected:
	bool IsReadyToClose() const;

private:
	const choices_t m_choices = {};
	std::string m_choice = "";
	const size_t m_default_choice = 0;

	MenuBlock* m_menu_block = nullptr;

	const choice_handlers_t& GetChoiceHandlers( const std::string& choice ) const;
};

}
}

