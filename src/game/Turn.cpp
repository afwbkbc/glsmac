#include "Turn.h"

namespace game {

Turn::Turn( const size_t id )
	: m_id( id ) {
	//
}

Turn::~Turn() {
	for ( auto& it : m_events ) {
		delete it;
	}
}

const size_t Turn::GetId() const {
	return m_id;
}

void Turn::AddEvent( const event::Event* event ) {
	m_events.push_back( event );
}

const Turn::events_t* const Turn::GetEvents() const {
	return &m_events;
}

}
