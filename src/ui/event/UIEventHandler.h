#pragma once

#include "base/Base.h"

#include <functional>

#include "UIEvent.h"

using namespace std;

namespace ui {
namespace event {

MAJOR_CLASS( UIEventHandler, base::Base )

	typedef function<void( const UIEvent::event_data_t* data )> handler_function_t;
	
	UIEventHandler( const handler_function_t func );
	
	void Execute( const UIEvent::event_data_t* data );

private:
	const handler_function_t m_func;
	
};

}
}
