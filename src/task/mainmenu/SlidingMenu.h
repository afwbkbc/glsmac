#pragma once

#include <vector>
#include <string>
#include <map>
#include <functional>

#include "MenuObject.h"

#include "ui/slidingmenu/MenuBlock.h"

namespace task {
namespace mainmenu {

#define CH( ... ) [ __VA_ARGS__ ] () -> void

CLASS( SlidingMenu, MenuObject )

	SlidingMenu( MainMenu* mainmenu, const std::string& title, const MenuBlock::choices_t& choices, const size_t default_choice = 0 );

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
	const MenuBlock::choices_t m_choices = {};
	std::string m_choice = "";
	const size_t m_default_choice = 0;

	MenuBlock* m_menu_block = nullptr;

	const MenuBlock::choice_handlers_t& GetChoiceHandlers( const std::string& choice ) const;
};

}
}

