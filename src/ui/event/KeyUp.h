#pragma once

#include "UIEvent.h"

namespace ui {
namespace event {

CLASS( KeyUp, UIEvent )

	KeyUp( const key_code_t code ) : UIEvent( EV_KEY_UP ) {
		m_flags |= EF_KEYBOARD;
		m_data.key.code = code;
	};
	
};

}
}
