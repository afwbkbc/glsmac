#include "Slot.h"

namespace game {

const Slot::slot_state_t Slot::GetState() const {
	return m_slot_state;
}

void Slot::Open() {
	ASSERT( m_slot_state == SS_CLOSED, "attempted to open non-closed slot" );
	m_slot_state = SS_OPEN;
}

Player* Slot::GetPlayer() const {
	return m_player;
}

Player* Slot::GetPlayerAndClose() {
	ASSERT( m_slot_state != SS_CLOSED, "attempted to close already closed slot" );
	Player* result = m_player;
	ASSERT( m_player->GetSlot() == this, "player-slot connection broken" );
	m_player->SetSlot( nullptr );
	m_player = nullptr;
	m_slot_state = SS_OPEN;
	return result;
}

void Slot::SetPlayer( Player* player ) {
	ASSERT( m_slot_state == SS_OPEN, "attempted to set player to non-open slot" );
	ASSERT( !player->GetSlot(), "attempted to set slot to player with non-empty slot" );
	m_player = player;
	player->SetSlot( this );
	m_slot_state = SS_PLAYER;
}

const types::Buffer Slot::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( m_slot_state );
	if ( m_slot_state == SS_PLAYER ) {
		buf.WriteString( m_player->Serialize().ToString() );
	}

	return buf;
}

void Slot::Unserialize( types::Buffer buf ) {
	m_slot_state = (slot_state_t) buf.ReadInt();
	if ( m_slot_state == SS_PLAYER ) {
		m_player = new Player();
		m_player->Unserialize( buf.ReadString() );
		m_player->SetSlot( this );
	}
}

}
