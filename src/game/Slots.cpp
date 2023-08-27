#include "Slots.h"

namespace game {

const size_t Slots::GetCount() const {
	return m_slots.size();
}

void Slots::Resize( const size_t size ) {
	m_slots.resize( size );
}

Slot& Slots::GetSlot( const size_t index ) {
	ASSERT( index < m_slots.size(), "slot index out of bounds" );
	return m_slots.at( index );
}

std::vector< Slot >& Slots::GetSlots() {
	return m_slots;
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
