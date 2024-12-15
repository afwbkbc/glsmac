#pragma once

#include "UIEvent.h"

namespace ui_legacy {
namespace event {

CLASS( MouseScroll, UIEvent )

	MouseScroll( const size_t x, const size_t y, const size_t scroll_y )
		: UIEvent( EV_MOUSE_SCROLL ) {
		m_flags |= EF_MOUSE;
		m_data.mouse.absolute.x = m_data.mouse.relative.x = x;
		m_data.mouse.absolute.y = m_data.mouse.relative.y = y;
		m_data.mouse.scroll_y = scroll_y;
	};

};

}
}
