#pragma once

#include "base/Base.h"

#include "Types.h"

namespace ui {
namespace event {

class UIEvent;

CLASS( UIEventHandler, base::Base )

	UIEventHandler( const handler_function_t func );

	bool Execute( const event_type_t event_type, const event_data_t* data );

private:
	const handler_function_t m_func;

};

}
}
