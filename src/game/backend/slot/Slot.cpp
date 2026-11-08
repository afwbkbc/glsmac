#include "Slot.h"

#include "engine/Engine.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/Player.h"
#include "game/backend/faction/Faction.h"

#include "gse/value/Int.h"

#include "game/backend/base/BaseManager.h"
#include "game/backend/base/production/Facility.h"
#include "game/backend/base/production/Custom.h"

namespace game {
namespace backend {
namespace slot {

Slot::Slot( const size_t index, State* state )
	: m_index( index )
	, m_state( state ) {}

Slot::~Slot() {
	for ( const auto& it : m_production_choices ) {
		delete it.second;
	}
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

	buf.WriteInt( m_production_choices.size() );
	for ( const auto& it : m_production_choices ) {
		buf.WriteString( it.first );
		it.second->Serialize( buf );
	}

	return buf;
}

void Slot::Deserialize( types::Buffer buf ) {
	m_slot_state = (slot_state_t)buf.ReadInt();
	if ( m_slot_state == SS_PLAYER ) {
		if ( !m_player_data.player ) {
			m_player_data.player = new Player( buf.ReadString() );
			m_player_data.player->SetSlot( this );
		}
		else {
			m_player_data.player->Deserialize( buf.ReadString() );
		}
		m_player_data.flags = buf.ReadInt();
	}
	m_linked_gsid = buf.ReadString();

	ASSERT( m_production_choices.empty(), "production choices not empty" );
	const size_t sz = buf.ReadInt();
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto id = buf.ReadString();
		m_production_choices.insert( { id, base::production::Production::Deserialize( buf ) } );
	}
}

void Slot::AddProductionChoice( GSE_CALLABLE, const std::string& id, const gse::value::object_properties_t& data ) {
	if ( m_production_choices.find( id ) != m_production_choices.end() ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Production choice '" + id + "' is already defined" );
	}
	const auto& type_it = data.find( "type" );
	if ( type_it == data.end() || type_it->second->type != gse::VT_STRING ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - type is not specified or is not a string" );
	}
	const auto& type = ( (gse::value::String*)type_it->second )->value;
	std::string hint = "";
	{
		const auto& hint_it = data.find( "hint" );
		if ( hint_it != data.end() ) {
			if ( hint_it->second->type != gse::VT_STRING ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - hint is not string" );
			}
			hint = ( (gse::value::String*)hint_it->second )->value;
		}
	}
	std::string intent = "";
	{
		const auto& intent_it = data.find( "intent" );
		if ( intent_it != data.end() ) {
			if ( intent_it->second->type != gse::VT_STRING ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - intent is not string" );
			}
			intent = ( (gse::value::String*)intent_it->second )->value;
		}
	}
	base::production::Production* production = nullptr;
	if ( type == "facility" ) {
		const auto& facility_id_it = data.find( "id" );
		if ( facility_id_it == data.end() || facility_id_it->second->type != gse::VT_STRING ) {
			GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - facility id is not specified or is not string" );
		}
		const auto& facility_id = ( (gse::value::String*)facility_id_it->second )->value;
		const auto* const facility = g_engine->GetGame()->GetBM()->GetFacilityDef( facility_id );
		if ( !facility ) {
			GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - facility id not found: " + facility_id );
		}
		production = new base::production::Facility( id, facility, hint, intent );
	}
	else if ( type == "custom" ) {
		const auto& render_it = data.find( "render" );
		if ( render_it == data.end() || render_it->second->type != gse::VT_OBJECT ) {
			GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - render is not specified or is not object" );
		}
		const auto& name_it = data.find( "name" );
		if ( name_it == data.end() || name_it->second->type != gse::VT_STRING ) {
			GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data -name is not specified or is not string" );
		}
		production = new base::production::Custom(
			id,
			base::RenderInfo::FromValue( GSE_CALL, ( (gse::value::Object*)render_it->second )->value ),
			( (gse::value::String*)name_it->second )->value,
			hint, intent
		);
	}
	else {
		GSE_ERROR( gse::EC.GAME_ERROR, "Unknown production choice type: " + type );
	}
	m_production_choices.insert( { id, production } );
}

const Slot::production_choices_t& Slot::GetProductionChoices() const {
	return m_production_choices;
}

const base::production::Production* const Slot::GetProductionChoice( const std::string& id ) const {
	const auto& it = m_production_choices.find( id );
	if ( it != m_production_choices.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}
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
