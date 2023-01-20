#pragma once

#include "UIEvent.h"

namespace ui {
namespace event {

CHILD_CLASS( KeyUp, UIEvent )

	KeyUp( const key_code_t code ) : UIEvent( EV_KEYUP ) {
		m_flags |= EF_KEYBOARD;
		m_data.key.code = code;
	};
	
};

}
}
