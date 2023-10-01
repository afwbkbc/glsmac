#pragma once

#include "UIEvent.h"

namespace ui {
namespace event {

CLASS( MouseDown, UIEvent )

	MouseDown( const size_t x, const size_t y, const uint8_t button )
		: UIEvent( EV_MOUSE_DOWN ) {
		m_flags |= EF_MOUSE;
		m_data.mouse.absolute.x = m_data.mouse.relative.x = x;
		m_data.mouse.absolute.y = m_data.mouse.relative.y = y;
		m_data.mouse.button = button;
	};

};

}
}
