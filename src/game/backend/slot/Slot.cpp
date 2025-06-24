#include "Slot.h"

#include "game/backend/State.h"
#include "game/backend/Player.h"
#include "game/backend/faction/Faction.h"

#include "gse/value/String.h"
#include "gse/value/Int.h"

namespace game {
namespace backend {
namespace slot {

Slot::Slot( const size_t index, const State* state )
	: m_index( index )
	, m_state( state ) {

}

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

const size_t Slot::GetIndex() const {
	return m_index;
}

const network::cid_t Slot::GetCid() const {
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
	m_player_data.flags = PF_NONE;
	if ( m_close_after_clear ) {
		m_slot_state = SS_CLOSED;
		m_close_after_clear = false;
	}
	else {
		m_slot_state = SS_OPEN;
	}
	return result;
}

void Slot::SetPlayer( Player* player, const network::cid_t cid, const std::string& remote_address ) {
	if ( m_slot_state == SS_PLAYER ) {
		ASSERT( m_player_data.player == player, "attempted to overwrite player with another player" );
	}
	else {
		ASSERT( m_slot_state == SS_OPEN, "attempted to set player to non-open slot" );
		ASSERT( !player->GetSlot(), "attempted to set slot to player with non-empty slot" );
		m_player_data.cid = cid;
		m_player_data.remote_address = remote_address;
		m_player_data.flags = PF_NONE;
		m_player_data.player = player;
		m_slot_state = SS_PLAYER;
		player->SetSlot( this );
	}
}

const std::string& Slot::GetLinkedGSID() const {
	return m_linked_gsid;
}

void Slot::SetLinkedGSID( const std::string& gsid ) {
	ASSERT( m_linked_gsid.empty(), "linked gsid already set" );
	ASSERT( !gsid.empty(), "linked gsid is empty" );
	Log( "Linking GSID: " + gsid );
	m_linked_gsid = gsid;
}

void Slot::UnsetLinkedGSID() {
	ASSERT( !m_linked_gsid.empty(), "linked gsid not set" );
	Log( "Unlinking GSID: " + m_linked_gsid );
	m_linked_gsid.clear();
}

const player_flag_t Slot::GetPlayerFlags() const {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to get player flags from non-player slot" );
	return m_player_data.flags;
}

void Slot::SetPlayerFlags( const player_flag_t flags ) {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to set player flags to non-player slot" );
	m_player_data.flags = flags;
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
	buf.WriteString( m_linked_gsid );

	return buf;
}

void Slot::Unserialize( types::Buffer buf ) {
	m_slot_state = (slot_state_t)buf.ReadInt();
	if ( m_slot_state == SS_PLAYER ) {
		if ( !m_player_data.player ) {
			m_player_data.player = new Player( buf.ReadString() );
			m_player_data.player->SetSlot( this );
		}
		else {
			m_player_data.player->Unserialize( buf.ReadString() );
		}
		m_player_data.flags = buf.ReadInt();
	}
	m_linked_gsid = buf.ReadString();
}

WRAPIMPL_BEGIN( Slot )
	ASSERT( m_slot_state == SS_PLAYER, "only player slots can be wrapped for now" );
	auto* player = m_player_data.player;
	ASSERT( player, "can't wrap slot without player" );
	return player->Wrap( GSE_CALL );
}

UNWRAPIMPL_PTR( Slot )

}
}
}
