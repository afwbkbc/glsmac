#include "Turn.h"

namespace game {
namespace backend {
namespace turn {

Turn::Turn() {
	//
}

Turn::~Turn() {
	Reset();
}

const bool Turn::IsActive() const {
	return m_is_active;
}

const size_t Turn::GetId() const {
	return m_id;
}

void Turn::AdvanceTurn( const size_t turn_id ) {
	ASSERT_NOLOG( !m_is_active, "turn already active" );
	m_id = turn_id;
	m_is_active = true;
}

const util::crc32::crc_t Turn::FinalizeAndChecksum() {
	ASSERT_NOLOG( m_is_active, "turn not active" );
	m_is_active = false;
	return util::crc32::CRC32::CalculateFromBuffer( event::LegacyEvent::SerializeMultiple( m_events ) );
}

void Turn::AddEvent( event::LegacyEvent* event ) {
	m_events.push_back( event );
}

const Turn::events_t* const Turn::GetEvents() const {
	return &m_events;
}

void Turn::Reset() {
	m_id = 0;
	m_is_active = false;
	for ( auto& it : m_events ) {
		delete it;
	}
	m_events.clear();
}

}
}
}
