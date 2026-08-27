#include "Slot.h"

#include <memory>

#include "engine/Engine.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/Player.h"
#include "game/backend/faction/Faction.h"

#include "gse/value/String.h"
#include "gse/value/Int.h"

namespace game {
namespace backend {
namespace slot {

Slot::Slot( const size_t index, State* state )
	: m_index( index )
	, m_state( state ) {}

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
	if ( !( m_player_data.flags & flag ) ) {
		m_player_data.flags |= flag;
		Update();
	}
}

const bool Slot::HasPlayerFlag( const player_flag_t flag ) const {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to get player flag on non-player slot" );
	return m_player_data.flags & flag;
}

void Slot::UnsetPlayerFlag( const player_flag_t flag ) {
	ASSERT( m_slot_state == SS_PLAYER, "attempted to unset player flag on non-player slot" );
	if ( m_player_data.flags & flag ) {
		m_player_data.flags &= ~flag;
		Update();
	}
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
		m_player_data.player = player;
		m_slot_state = SS_PLAYER;
		player->SetSlot( this );
	}
	m_player_data.cid = cid;
	m_player_data.remote_address = remote_address;
	m_player_data.flags = PF_NONE;
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
	if ( m_player_data.flags != flags ) {
		m_player_data.flags = flags;
		Update();
	}
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

void Slot::Deserialize( types::Buffer buf ) {
	const auto serialized_state = buf.ReadInt();
	if ( serialized_state < SS_CLOSED || serialized_state > SS_PLAYER ) {
		THROW( "invalid serialized slot state: " + std::to_string( serialized_state ) );
	}
	const auto slot_state = static_cast< slot_state_t >( serialized_state );
	std::string serialized_player;
	player_flag_t player_flags = PF_NONE;
	if ( slot_state == SS_PLAYER ) {
		serialized_player = buf.ReadString();
		player_flags = buf.ReadInt< player_flag_t >( "slot player flags" );
		if ( player_flags & ~PF_ALL ) {
			THROW( "invalid serialized slot player flags" );
		}
	}
	const auto linked_gsid = buf.ReadString();
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized slot" );
	}

	if ( slot_state == SS_PLAYER ) {
		if ( !m_player_data.player ) {
			auto player = std::make_unique< Player >( types::Buffer( serialized_player ) );
			m_player_data.player = player.release();
			m_player_data.player->SetSlot( this );
		}
		else {
			m_player_data.player->Deserialize( types::Buffer( serialized_player ) );
		}
		m_player_data.flags = player_flags;
	}
	else {
		if ( m_player_data.player ) {
			THROW( "serialized non-player slot still owns a player" );
		}
		m_player_data.cid = 0;
		m_player_data.remote_address.clear();
		m_player_data.flags = PF_NONE;
	}
	m_slot_state = slot_state;
	m_close_after_clear = false;
	m_linked_gsid = linked_gsid;
}

WRAPIMPL_BEGIN( Slot )
	ASSERT( m_slot_state == SS_PLAYER, "only player slots can be wrapped for now" );
	auto* player = m_player_data.player;
	ASSERT( player, "can't wrap slot without player" );
	return player->Wrap( GSE_CALL );
}

UNWRAPIMPL_PTR( Slot )

void Slot::Update() {
	auto* const game = g_engine->GetGame();
	ASSERT( game, "game not set" );
	m_state->WithGSE(
		m_state, [ this, game ]( GSE_CALLABLE ) {
			game->Trigger(
				GSE_CALL, "player_update", ARGS_F( this ) {
					{
						"player", Wrap( GSE_CALL )
					}
				}; }
			);
		}
	);

}

}
}
}
