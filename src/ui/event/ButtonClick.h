#pragma once

#include "UIEvent.h"

namespace ui {
namespace event {

CHILD_CLASS( ButtonClick, UIEvent )

	ButtonClick( const size_t x, const size_t y, const uint8_t button ) : UIEvent( EV_BUTTON_CLICK ) {
		m_flags |= EF_MOUSE;
		m_data.mouse.x = x;
		m_data.mouse.y = y;
		m_data.mouse.button = button;
	};
	
};

}
}
