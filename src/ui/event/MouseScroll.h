#pragma once

#include "UIEvent.h"

namespace ui {
namespace event {

CLASS( MouseScroll, UIEvent )

	MouseScroll( const size_t y ) : UIEvent( EV_MOUSE_SCROLL ) {
		m_flags |= EF_MOUSE;
		m_data.mouse.y = y;
	};
	
};

}
}
