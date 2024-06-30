#pragma once

#include <string>
#include <atomic>
#include <mutex>
#include <functional>

#include "Module.h"

#include "ui/Types.h"

#include "util/Timer.h"

namespace ui {

namespace object {
class Section;
class Label;
class Button;
}

namespace module {

CLASS( Loader, Module )

	Loader( UI* ui );
	~Loader();

	void Show( const std::string& text, const loader_cancel_handler_t on_cancel = 0 );
	void Hide();

	const bool IsActive() const;

	void SetText( const std::string& loading_text ); // mt supported
	void SetIsCancelable( const bool is_cancelable );

	void ProcessEvent( event::UIEvent* event ) override;

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

	object::Section* m_section = nullptr;
	object::Label* m_label = nullptr;
	object::Button* m_button_cancel = nullptr;

	loader_cancel_handler_t m_on_cancel = 0;

};

}
}
