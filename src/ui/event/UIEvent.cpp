#include "UIEvent.h"

namespace ui {
namespace event {

UIEvent::UIEvent( const UIEvent* other )
	: base::Base()
	, m_type( other->m_type )
	, m_flags( other->m_flags )
	, m_data( other->m_data )
	, m_is_processed( false )
{
	ASSERT( !other->IsProcessed(), "copying from processed event" );
}

UIEvent::UIEvent( const event_type_t type )
	: base::Base()
	, m_type( type )
{
	//
}

void UIEvent::SetProcessed() {
	ASSERT( !m_is_processed, "event already processed" );
	m_is_processed = true;
}

const bool UIEvent::IsProcessed() const {
	return m_is_processed;
}

}
}
