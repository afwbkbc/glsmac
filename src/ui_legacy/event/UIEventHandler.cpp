#include "UIEventHandler.h"

namespace ui_legacy {
namespace event {

UIEventHandler::UIEventHandler( const handler_function_t func )
	: m_func( func ) {

}

bool UIEventHandler::Execute( const event_type_t event_type, const event_data_t* data ) {
	return m_func( event_type, data );
}

}
}
