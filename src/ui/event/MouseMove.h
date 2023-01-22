#pragma once

#include "UIEvent.h"

namespace ui {
namespace event {

CLASS( MouseMove, UIEvent )

	MouseMove( const size_t x, const size_t y ) : UIEvent( EV_MOUSE_MOVE ) {
		m_flags |= EF_MOUSE;
		m_data.mouse.x = x;
		m_data.mouse.y = y;
	};
	
};

}
}
