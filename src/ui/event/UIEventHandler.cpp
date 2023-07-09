#include "UIEventHandler.h"

namespace ui {
namespace event {

UIEventHandler::UIEventHandler( const handler_function_t func ) : m_func( func ) {
	
}

bool UIEventHandler::Execute( const UIEvent::event_type_t event_type, const UIEvent::event_data_t* data ) {
	return m_func( event_type, data );
}

}
}
