#pragma once

#include <vector>
#include <string>

#include "ui/object/UIContainer.h"

#include "Types.h"

#include "util/Timer.h"

namespace ui::object {
class SoundEffect;
}

namespace task {
namespace mainmenu {

class SlidingMenu;
class MenuItem;

CLASS( MenuBlock, ui::object::UIContainer )
	MenuBlock( SlidingMenu* menu );

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

	ui::object::SoundEffect* m_sound = nullptr;
};

}
}
