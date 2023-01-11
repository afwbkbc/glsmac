#pragma once

#include "base/Base.h"

#include "UIEvent.h"

namespace ui {
namespace event {

CHILD_CLASS( MouseMove, UIEvent )

	MouseMove( const size_t x, const size_t y ) : UIEvent( EV_MOUSEMOVE ) {
		m_flags |= EF_MOUSE;
		m_data.mouse.x = x;
		m_data.mouse.y = y;
	};
	
};

}
}
