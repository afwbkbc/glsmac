#include "UIEvent.h"

namespace ui {
namespace event {

UIEvent::UIEvent( const event_type_t type )
	: base::Base()
	, m_type( type )
{
	//
}

}
}
