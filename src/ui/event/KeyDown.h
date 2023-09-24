#pragma once

#include "UIEvent.h"

namespace ui {
namespace event {

CLASS( KeyDown, UIEvent )

	KeyDown( const key_code_t code, const char key, key_modifier_t modifiers )
		: UIEvent( EV_KEY_DOWN ) {
		m_flags |= EF_KEYBOARD;
		m_data.key.code = code;
		m_data.key.key = key;
		m_data.key.is_printable = ( key > 0 );
		m_data.key.modifiers = modifiers;
	};

};

}
}
