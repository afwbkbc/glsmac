#pragma once

#include "UIEvent.h"

namespace ui_legacy {
namespace event {

CLASS( KeyPress, UIEvent )

	KeyPress( const key_code_t code )
		: UIEvent( EV_KEY_PRESS ) {
		m_flags |= EF_KEYBOARD;
		m_data.key.code = code;
	};

};

}
}
