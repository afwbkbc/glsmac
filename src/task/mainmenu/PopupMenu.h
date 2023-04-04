#pragma once

#include <string>
#include <unordered_set>

#include "MenuObject.h"

#include "ui/object/Panel.h"
#include "ui/object/Section.h"
#include "ui/object/Button.h"

using namespace ui;
namespace ui {
	using namespace object;
}

namespace task {
namespace mainmenu {
	
class MainMenu;

CLASS( PopupMenu, MenuObject )
	PopupMenu( MainMenu *mainmenu, const std::string& title );

	enum popup_flag_t : uint8_t {
		PF_HAS_OK,
		PF_HAS_CANCEL,
	};
	
	virtual void Show();
	virtual void Hide();
	
	virtual void OnNext() {};
	
protected:
	// true means cancel is allowed, otherwise do nothing
	virtual bool OnCancel() { return true; };
	
	void SetFlags( const std::unordered_set< popup_flag_t > flags );
	void SetWidth( const size_t width );
	void SetHeight( const size_t height );
	
	Panel* m_body = nullptr; // child classes can add elements here
	
	void Resize();
	
private:
	Section* m_frame = nullptr;
		Button* m_button_ok = nullptr;
		Button* m_button_cancel = nullptr;
	
	std::unordered_set< popup_flag_t > m_flags = {};
	size_t m_width = 0;
	size_t m_height = 0;
	
	bool HasFlag( const popup_flag_t flag ) const;
};

}
}

#include "MainMenu.h"
