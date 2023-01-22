#pragma once

#include "../PopupMenu.h"

#include <string>
#include <functional>

#include "ui/object/Section.h"
#include "ui/object/Label.h"
#include "ui/object/Button.h"

namespace game {
namespace mainmenu {

class Loading;
	
typedef function<bool( Loading* loader )> loading_handler_t;

#define LH( ... ) [ __VA_ARGS__ ] ( Loading* loader ) -> bool

CLASS( Loading, PopupMenu )

	Loading( MainMenu* mainmenu );

	void Show();
	void Hide();

	void SetOnStart( const loading_handler_t handler );
	void SetOnIterate( const loading_handler_t handler );
	void SetOnCancel( const loading_handler_t handler );
	
	void OnCancel();

private:
	Section* m_section = nullptr;
	Label* m_label = nullptr;
	Button* m_cancel = nullptr;
	
	struct {
		loading_handler_t on_start = nullptr;
		loading_handler_t on_iterate = nullptr;
		loading_handler_t on_cancel = nullptr;
	} m_handlers;

};
	
}
}
