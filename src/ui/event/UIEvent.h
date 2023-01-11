#pragma once

#include "base/Base.h"

namespace ui {
namespace event {

MAJOR_CLASS( UIEvent, base::Base )

	enum event_type_t {
		EV_NONE,
		EV_MOUSEMOVE,
		EV_MOUSEDOWN,
		EV_KEYDOWN,
	};
	
	// event flags
	const static uint8_t EF_NONE = 0;
	const static uint8_t EF_MOUSE = 1;
	const static uint8_t EF_KEYBOARD = 2;

	enum key_code_t {
		K_NONE,
		K_ESCAPE,
	};
	
	typedef union {
		struct {
			size_t x;
			size_t y;
			uint8_t button;
		} mouse;
		struct {
			key_code_t code;
		} key;
	} event_data_t;
	
	const event_type_t m_type = EV_NONE;
	uint8_t m_flags = EF_NONE;
	event_data_t m_data = {};
	
protected:
	UIEvent( const event_type_t type );
	
	
};

}
}
