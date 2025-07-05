#include "Slots.h"

namespace game {
namespace backend {
namespace slot {

Slots::Slots( State* state )
	: m_state( state ) {}

const size_t Slots::GetCount() const {
	return m_slots.size();
}

void Slots::Resize( const size_t size ) {
	if ( m_slots.size() > size ) {
		m_slots.erase( m_slots.begin() + size, m_slots.end() );
	}
	for ( size_t i = m_slots.size() ; i < size ; i++ ) {
		m_slots.push_back(
			{
				i,
				m_state
			}
		);
	}
}

Slot& Slots::GetSlot( const size_t index ) {
	ASSERT( index < m_slots.size(), "slot index out of bounds" );
	return m_slots.at( index );
}

std::vector< Slot >& Slots::GetSlots() {
	return m_slots;
}

void Slots::Clear() {
	m_slots.clear();
}

const types::Buffer Slots::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( m_slots.size() );

	for ( auto& slot : m_slots ) {
		buf.WriteString( slot.Serialize().ToString() );
	}

	return buf;
}

void Slots::Unserialize( types::Buffer buf ) {
	ASSERT( m_slots.empty(), "unserialize on non-empty slots" );
	Resize( buf.ReadInt() );

	for ( auto& slot : m_slots ) {
		slot.Unserialize( buf.ReadString() );
	}
}

}
}
}
