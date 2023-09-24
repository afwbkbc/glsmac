#pragma once

#include <vector>
#include <string>

#include "ui/object/UIContainer.h"

#include "ui/object/SoundEffect.h"

#include "types/Texture.h"
#include "types/Font.h"
#include "util/Timer.h"

#include "MenuItem.h"

using namespace ui::object;
using namespace ui::event;

namespace task {
namespace mainmenu {

class SlidingMenu;

CLASS( MenuBlock, UIContainer )
	MenuBlock( SlidingMenu* menu );

	// TODO: refactor
	typedef std::function< void() > choice_handler_t;
	struct choice_handlers_t {
		choice_handler_t on_click = 0;
		choice_handler_t on_select = 0;
	};
	typedef std::vector< std::pair< std::string, choice_handlers_t > > choices_t;

	// how much pixels to shift when fully closed
	static constexpr coord_t MENU_CLOSED_POSITION = 400;

	// per ms
	static constexpr size_t SLIDING_SPEED = 5;

	void Create() override;
	void Destroy() override;
	void Iterate() override;

	void AddItem( const std::string& text, const choice_handlers_t& handlers );
	void AddTitle( const std::string& text );
	void SelectItem( const size_t index );

	const std::string GetChoice() const;
	void SetChoice( const std::string& choice );

	void GoBack();
	bool IsSliding();

protected:
	friend class MenuItem;

	void OnItemClick( const std::string& choice );

	void SetActiveItem( const size_t index );

	choices_t m_items = {};
	std::string m_title = "";

	SlidingMenu* m_menu = nullptr;
	std::vector< MenuItem* > m_menu_items = {};
	MenuItem* m_selected_item = nullptr;
	size_t m_selected_item_index = 0;

	std::string m_clicked_choice = "";

	ssize_t m_slide_value = -MENU_CLOSED_POSITION;
	ssize_t m_slide_change = SLIDING_SPEED;
	util::Timer m_slide_timer;

	SoundEffect* m_sound = nullptr;
};

}
}

#include "../../SlidingMenu.h"
