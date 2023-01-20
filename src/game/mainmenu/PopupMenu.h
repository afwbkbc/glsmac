#pragma once

#include "MenuObject.h"

#include <string>
#include <unordered_set>

#include "ui/object/Panel.h"
#include "ui/object/Button.h"

using namespace std;

using namespace ui;
namespace ui {
	using namespace object;
}

namespace game {
namespace mainmenu {
	
class MainMenu;

class PopupMenu : public MenuObject {
public:
	PopupMenu( MainMenu *mainmenu, const string& title );

	enum popup_flag_t : uint8_t {
		PF_HAS_OK,
		PF_HAS_CANCEL,
	};
	
	virtual void Show();
	virtual void Hide();
	
	virtual void OnNext() {};
	
protected:
	
	void SetFlags( const unordered_set< popup_flag_t > flags );
	void SetWidth( const size_t width );
	void SetHeight( const size_t height );
	
	Panel* m_body = nullptr; // child classes can add elements here
	
private:
	Panel* m_frame = nullptr;
		Panel* m_titleframe = nullptr;
			Panel* m_titlebar = nullptr;
				Label* m_titlelabel = nullptr;
		Button* m_button_ok = nullptr;
		Button* m_button_cancel = nullptr;
	
	unordered_set< popup_flag_t > m_flags = {};
	size_t m_width = 0;
	size_t m_height = 0;
	
	bool HasFlag( const popup_flag_t flag ) const;
};

}
}

#include "MainMenu.h"
