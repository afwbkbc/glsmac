#pragma once

#include <string>

#include "UIModule.h"

#include "ui/object/Section.h"
#include "ui/object/Button.h"

#include "util/Timer.h"

namespace ui {
namespace module {

class Loader;
	
typedef function<bool( ui::module::Loader* loader )> loading_handler_t;
#define LH( ... ) [ __VA_ARGS__ ] ( ui::module::Loader* loader ) -> bool

CLASS( Loader, UIModule )
	
	void Start();
	void Stop();

	const bool IsRunning() const;
	
	void SetOnStart( const loading_handler_t handler );
	void SetOnIterate( const loading_handler_t handler );
	void SetOnStop( const loading_handler_t handler );
	
	void SetLoadingText( const string& loading_text );
	
	const string GetDots();
	
private:
	
	// . -> .. -> ... animation
	util::Timer m_dots_timer;
	uint8_t m_dots_count = 0;
	
	bool m_is_iterating = false;
	
	string m_loading_text = "";
	
	Section* m_section = nullptr;
	Label* m_label = nullptr;
	Button* m_button_cancel = nullptr;
	
	struct {
		loading_handler_t on_start = nullptr;
		loading_handler_t on_iterate = nullptr;
		loading_handler_t on_stop = nullptr;
	} m_handlers;

};
	
}
}
