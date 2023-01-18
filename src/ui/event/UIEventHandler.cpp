#include "UIEventHandler.h"

namespace ui {
namespace event {

UIEventHandler::UIEventHandler( const handler_function_t func ) : m_func( func ) {
	
}

void UIEventHandler::Execute( const UIEvent::event_data_t* data ) {
	m_func( data );
}

}
}
