#pragma once

#include "base/Base.h"

namespace ui {
namespace event {

MAJOR_CLASS( UIEvent, base::Base )

	enum event_type_t {
		EV_NONE,
		EV_MOUSEMOVE,
	};
	
	// event flags
	const static uint8_t EF_NONE = 0;
	const static uint8_t EF_MOUSE = 1;

	typedef union {
		struct {
			size_t x;
			size_t y;
		} mouse;
	} event_data_t;
	
	const event_type_t m_type = EV_NONE;
	uint8_t m_flags = EF_NONE;
	event_data_t m_data;
	
protected:
	UIEvent( const event_type_t type );
	
	
};

}
}
