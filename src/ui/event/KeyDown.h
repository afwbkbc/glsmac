#pragma once

#include "UIEvent.h"

namespace ui {
namespace event {

CHILD_CLASS( KeyDown, UIEvent )

	KeyDown( const key_code_t code ) : UIEvent( EV_KEY_DOWN ) {
		m_flags |= EF_KEYBOARD;
		m_data.key.code = code;
	};
	
};

}
}
