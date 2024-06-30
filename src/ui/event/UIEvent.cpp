#include "UIEvent.h"

namespace ui {
namespace event {

UIEvent::UIEvent( const UIEvent* other )
	: common::Class()
	, m_type( other->m_type )
	, m_flags( other->m_flags )
	, m_data( other->m_data )
	, m_is_processed( false )
	, m_is_mouseover_happened( other->m_is_mouseover_happened ) {
	ASSERT( !other->IsProcessed(), "copying from processed event" );
}

UIEvent::UIEvent( const event_type_t type )
	: common::Class()
	, m_type( type ) {
	//
}

void UIEvent::SetProcessed() {
	ASSERT( !m_is_processed, "event already processed" );
	m_is_processed = true;
}

const bool UIEvent::IsProcessed() const {
	return m_is_processed;
}

void UIEvent::SetMouseOverHappened() {
	m_is_mouseover_happened = true;
}

const bool UIEvent::IsMouseOverHappened() {
	return m_is_mouseover_happened;
}

}
}
