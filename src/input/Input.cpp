#include "Input.h"

namespace input {

void Input::AddHandler( const void* const owner, const input_handler_t& handler ) {
	ASSERT( m_input_handlers.find( owner ) == m_input_handlers.end(), "handler already set" );
	m_input_handlers.insert(
		{
			owner,
			handler
		}
	);
}

void Input::RemoveHandler( const void* const owner ) {
	ASSERT( m_input_handlers.find( owner ) != m_input_handlers.end(), "handler not set" );
	m_input_handlers.erase( owner );
}

void Input::ProcessEvent( const Event& event ) {
	for ( const auto& it : m_input_handlers ) {
		it.second( event );
	}
}

}
