#pragma once

#include <string>
#include <unordered_set>
#include <cstdint>

#include "MenuObject.h"

namespace ui_legacy::object {
class Panel;
class Section;
class Button;
}

namespace task {
namespace mainmenu {

class MainMenu;

CLASS( PopupMenu, MenuObject )
	PopupMenu( MainMenu* mainmenu, const std::string& title );

	enum popup_flag_t : uint8_t {
		PF_HAS_OK,
		PF_HAS_CANCEL,
	};

	virtual void Show() override;
	virtual void Hide() override;

	virtual void OnNext() {};

protected:
	// true means cancel is allowed, otherwise do nothing
	virtual bool OnCancel() { return true; };

	void SetFlags( const std::unordered_set< popup_flag_t > flags );
	void SetWidth( const size_t width );
	void SetHeight( const size_t height );

	ui_legacy::object::Panel* m_body = nullptr; // child classes can add elements here

	void Resize();

	const bool IsShown() const;

	ui_legacy::object::Section* m_frame = nullptr;

private:
	ui_legacy::object::Button* m_button_ok = nullptr;
	ui_legacy::object::Button* m_button_cancel = nullptr;

	std::unordered_set< popup_flag_t > m_flags = {};
	size_t m_width = 0;
	size_t m_height = 0;

	bool m_is_shown = false;

	bool HasFlag( const popup_flag_t flag ) const;
};

}
}
