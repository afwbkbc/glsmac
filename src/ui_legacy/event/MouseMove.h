#pragma once

#include "UIEvent.h"

namespace ui_legacy {
namespace event {

CLASS( MouseMove, UIEvent )

	MouseMove( const size_t x, const size_t y )
		: UIEvent( EV_MOUSE_MOVE ) {
		m_flags |= EF_MOUSE;
		m_data.mouse.absolute.x = m_data.mouse.relative.x = x;
		m_data.mouse.absolute.y = m_data.mouse.relative.y = y;
	};

};

}
}
