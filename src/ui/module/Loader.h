#pragma once

#include <string>
#include <atomic>
#include <mutex>

#include "Module.h"

#include "ui/object/Section.h"
#include "ui/object/Button.h"

#include "util/Timer.h"

namespace ui {
namespace module {

// return true if loaded should hide immediately
// return false if you want cancel button hidden but loader staying (you'll need to close loader manually later)
typedef std::function<bool()> loader_cancel_handler_t;
#define LCH( ... ) [ __VA_ARGS__ ] () -> bool

CLASS( Loader, Module )
	
	Loader( UI* ui );
	~Loader();
	
	void Show( const std::string& text, const loader_cancel_handler_t on_cancel = 0 );
	void Hide();
	
	void SetText( const std::string& loading_text ); // mt supported
	void SetIsCancelable( const bool is_cancelable );
	
	void ProcessEvent( event::UIEvent* event );
	
private:
	
	void Start();
	void Stop();

	void Cancel();
	
	const std::string GetDots();
	
	bool m_is_cancelable = true;
	
	// . -> .. -> ... animation
	util::Timer m_dots_timer;
	uint8_t m_dots_count = 0;
	
	// can be set by other threads
	struct {
		std::atomic< bool > is_changed = false;
		std::mutex mutex;
		std::string value = "";
	} m_loading_text_change = {};
	std::string m_loading_text = "";
	
	Section* m_section = nullptr;
	Label* m_label = nullptr;
	Button* m_button_cancel = nullptr;
	
	loader_cancel_handler_t m_on_cancel = 0;
	
};
	
}
}
