#include "Slot.h"

namespace game {

const Slot::slot_state_t Slot::GetState() const {
	return m_slot_state;
}

void Slot::Open() {
	ASSERT( m_slot_state == SS_CLOSED, "attempted to open non-closed slot" );
	m_slot_state = SS_OPEN;
}

void Slot::Close() {
	ASSERT( m_slot_state == SS_OPEN, "attempted to close non-open slot" );
	m_slot_state = SS_CLOSED;
}

void Slot::SetCloseAfterClear() {
	ASSERT( m_slot_state == SS_PLAYER, "SetCloseAfterClear on non-player slot" );
	m_close_after_clear = true;
}

Player* Slot::GetPlayer() const {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to get player from non-player slot" );
	return m_player_data.player;
}

size_t Slot::GetCid() const {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to get cid from non-player slot" );
	return m_player_data.cid;
}

const std::string& Slot::GetRemoteAddress() const {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to get remote address from non-player slot" );
	return m_player_data.remote_address;
}

void Slot::SetPlayerFlag( const player_flag_t flag ) {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to set player flag on non-player slot" );
	m_player_data.flags |= flag;
}

const bool Slot::HasPlayerFlag( const player_flag_t flag ) const {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to get player flag on non-player slot" );
	return m_player_data.flags & flag;
}

void Slot::UnsetPlayerFlag( const player_flag_t flag ) {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to unset player flag on non-player slot" );
	m_player_data.flags &= ~flag;
}

Player* Slot::GetPlayerAndClose() {
	ASSERT( m_slot_state != SS_CLOSED, "attempted to close already closed slot" );
	Player* result = m_player_data.player;
	ASSERT( m_player_data.player->GetSlot() == this, "player-slot connection broken" );
	m_player_data.player->SetSlot( nullptr );
	m_player_data.player = nullptr;
	m_player_data.cid = 0;
	m_player_data.remote_address = "";
	if ( m_close_after_clear ) {
		m_slot_state = SS_CLOSED;
		m_close_after_clear = false;
	}
	else {
		m_slot_state = SS_OPEN;
	}
	return result;
}

void Slot::SetPlayer( Player* player, const size_t cid, const std::string& remote_address ) {
	ASSERT( m_slot_state == SS_OPEN, "attempted to set player to non-open slot" );
	ASSERT( !player->GetSlot(), "attempted to set slot to player with non-empty slot" );
	m_player_data.player = player;
	m_player_data.cid = cid;
	m_player_data.remote_address = remote_address;
	player->SetSlot( this );
	m_slot_state = SS_PLAYER;
}

const types::Buffer Slot::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( m_slot_state );
	if ( m_slot_state == SS_PLAYER ) {
		buf.WriteString( m_player_data.player->Serialize().ToString() );
		// not sending cid
		// not sending remote address
		buf.WriteInt( m_player_data.flags );
	}

	return buf;
}

void Slot::Unserialize( types::Buffer buf ) {
	m_slot_state = (slot_state_t)buf.ReadInt();
	if ( m_slot_state == SS_PLAYER ) {
		if ( !m_player_data.player ) {
			m_player_data.player = new Player();
			m_player_data.player->SetSlot( this );
		}
		m_player_data.player->Unserialize( buf.ReadString() );
		m_player_data.flags = buf.ReadInt();
	}
}

}
