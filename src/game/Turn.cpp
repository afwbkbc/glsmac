#include "Turn.h"

namespace game {

Turn::Turn() {
	//
}

Turn::~Turn() {
	for ( auto& it : m_events ) {
		delete it;
	}
}

const bool Turn::IsActive() const {
	return m_is_active;
}

void Turn::Activate() {
	ASSERT_NOLOG( !m_is_active, "turn already active" );
	m_is_active = true;
}

void Turn::Deactivate() {
	ASSERT_NOLOG( m_is_active, "turn not active" );
	m_is_active = false;
}

void Turn::AddEvent( const event::Event* event ) {
	m_events.push_back( event );
}

const Turn::events_t* const Turn::GetEvents() const {
	return &m_events;
}

}
