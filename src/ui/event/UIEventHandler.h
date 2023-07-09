#pragma once

#include <functional>

#include "base/Base.h"

#include "UIEvent.h"

namespace ui {
namespace event {

#define EH(...) [__VA_ARGS__] ( const UIEvent::event_type_t event_type, const UIEvent::event_data_t* data ) -> bool
	
CLASS( UIEventHandler, base::Base )

	typedef std::function<bool( const UIEvent::event_type_t event_type, const UIEvent::event_data_t* data )> handler_function_t;
	
	UIEventHandler( const handler_function_t func );
	
	bool Execute( const UIEvent::event_type_t event_type, const UIEvent::event_data_t* data );

private:
	const handler_function_t m_func;
	
};

}
}
