#pragma once

#include "base/Base.h"

namespace ui {
namespace event {

CLASS( UIEvent, base::Base )

	enum event_type_t {
		EV_NONE,
		EV_MOUSE_MOVE,
		EV_MOUSE_OVER,
		EV_MOUSE_OUT,
		EV_MOUSE_DOWN,
		EV_MOUSE_UP,
		EV_KEY_DOWN,
		EV_KEY_UP,
		EV_KEY_PRESS,
		EV_BUTTON_CLICK,
		EV_BUTTON_DOUBLE_CLICK,
		EV_CHANGE,
	};
	
	// event flags
	const static uint8_t EF_NONE = 0;
	const static uint8_t EF_MOUSE = 1;
	const static uint8_t EF_KEYBOARD = 2;

	enum mouse_button_t : uint8_t {
		M_NONE,
		M_LEFT,
		M_RIGHT,
		M_MIDDLE,
	};
	
	enum key_code_t {
		K_NONE,
		K_UP,
		K_DOWN,
		K_LEFT,
		K_RIGHT,
		K_ENTER,
		K_TAB,
		K_BACKSPACE,
		K_ESCAPE,
		K_GRAVE,
	};
	
	typedef union {
		struct {
			size_t x;
			size_t y;
			uint8_t button;
		} mouse;
		struct {
			bool is_printable;
			key_code_t code;
			char key;
		} key;
	} event_data_t;
	
	const event_type_t m_type = EV_NONE;
	uint8_t m_flags = EF_NONE;
	event_data_t m_data = {};
	
	// event will stop propagating after this
	void SetProcessed();
	const bool IsProcessed() const;
	
	// copy from other event
	UIEvent( const UIEvent* other );
	
protected:
	UIEvent( const event_type_t type );
	
	
private:
	bool m_is_processed = false;
	
};

}
}
