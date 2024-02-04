#include "Turn.h"

namespace game {

Turn::Turn( const size_t year )
	: m_year( year ) {

}

const size_t Turn::GetYear() const {
	return m_year;
}

void Turn::AddEvent( const event::Event* event ) {
	m_events.push_back( event );
}

const Turn::events_t* const Turn::GetEvents() const {
	return &m_events;
}

}
