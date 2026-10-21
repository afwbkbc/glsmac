#include "Player.h"

#include "game/backend/faction/Faction.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/State.h"
#include "game/backend/slot/Slots.h"

#include "gse/value/String.h"
#include "gse/value/Bool.h"

#include "engine/Engine.h"
#include "Game.h"

#include "game/backend/base/BaseManager.h"
#include "game/backend/base/production/Facility.h"
#include "game/backend/base/production/Custom.h"

namespace game {
namespace backend {

Player::Player( types::Buffer buf ) {
	Player::Deserialize( buf );
}

Player::Player(
	const std::string& name,
	const role_t role,
	faction::Faction* faction,
	const std::string& difficulty_level
)
	: m_name( name )
	, m_role( role )
	, m_faction( faction )
	, m_difficulty_level( difficulty_level ) {}

Player::Player( const Player* const other ) {
	*this = *other;
}

Player::~Player() {
	for ( const auto& it : m_production_choices ) {
		delete it.second;
	}
}

const std::string& Player::GetPlayerName() const {
	return m_name;
}

const std::string Player::GetFullName() const {
	return GetPlayerName() + " (" + m_faction->m_name + ")";
}

void Player::Connect() {
	ASSERT( !m_is_connected, "player already connected" );
	m_is_connected = true;
}

void Player::Disconnect() {
	ASSERT( m_is_connected, "player not connected" );
	m_is_connected = false;
}

const bool Player::IsConnected() const {
	return m_is_connected;
}

void Player::SetFaction( faction::Faction* faction ) {
	// TODO: validate?
	m_faction = faction;
}

void Player::ClearFaction() {
	m_faction = nullptr;
}

faction::Faction* Player::GetFaction() {
	return m_faction;
}

void Player::SetDifficultyLevel( const std::string& difficulty_level ) {
	// TODO: validate?
	m_difficulty_level = difficulty_level;
}

const std::string& Player::GetDifficultyLevel() const {
	return m_difficulty_level;
}

void Player::SetSlot( slot::Slot* slot ) {
	m_slot = slot;
	if ( m_slot ) {
		m_slotnum = m_slot->GetIndex();
	}
}

slot::Slot* Player::GetSlot() const {
	return m_slot;
}

const Player::role_t Player::GetRole() const {
	return m_role;
}

const bool Player::IsTurnCompleted() const {
	return m_is_turn_completed;
}

void Player::CompleteTurn() {
	m_is_turn_completed = true;
}

void Player::UncompleteTurn() {
	m_is_turn_completed = false;
}

void Player::AddProductionChoice( GSE_CALLABLE, const std::string& id, const gse::value::object_properties_t& data ) {
	if ( m_production_choices.find( id ) != m_production_choices.end() ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Production choice '" + id + "' is already defined" );
	}
	const auto& type_it = data.find( "type" );
	if ( type_it == data.end() || type_it->second->type != gse::VT_STRING ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - type is not specified or is not a string" );
	}
	const auto& type = ((gse::value::String*)type_it->second)->value;
	std::string hint = "";
	{
		const auto& hint_it = data.find( "hint" );
		if ( hint_it != data.end() ) {
			if ( hint_it->second->type != gse::VT_STRING ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - hint is not string" );
			}
			hint = ((gse::value::String*)hint_it->second)->value;
		}
	}
	base::production::Production* production = nullptr;
	if ( type == "facility" ) {
		const auto& facility_id_it = data.find( "id" );
		if ( facility_id_it == data.end() || facility_id_it->second->type != gse::VT_STRING ) {
			GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - facility id is not specified or is not string" );
		}
		const auto& facility_id = ((gse::value::String*)facility_id_it->second)->value;
		const auto* const facility = g_engine->GetGame()->GetBM()->GetFacilityDef( facility_id );
		if ( !facility ) {
			GSE_ERROR( gse::EC.GAME_ERROR, "Production choice data - facility id not found: " + facility_id );
		}
		production = new base::production::Facility( facility, hint );
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
			base::RenderInfo::FromValue( GSE_CALL, ((gse::value::Object*)render_it->second)->value ),
			((gse::value::String*)name_it->second)->value,
			hint
		);
	}
	else {
		GSE_ERROR( gse::EC.GAME_ERROR, "Unknown production choice type: " + type );
	}
	m_production_choices.insert({ id, production });
}

const Player::production_choices_t& Player::GetProductionChoices() const {
	return m_production_choices;
}

WRAPIMPL_BEGIN( Player )
	auto* const game = g_engine->GetGame();
	WRAPIMPL_PROPS
			{
				"id",
				VALUE( gse::value::Int, , m_slotnum )
			},
			{
				"type",
				VALUE( gse::value::String, , "human" )
			},
			{
				"name",
				VALUE( gse::value::String, , m_name )
			},
			{
				"is_ready",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return VALUE( gse::value::Bool, , m_slot->HasPlayerFlag( ::game::backend::slot::PF_READY ) );
				} )
			},
			{
				"set_ready",
				NATIVE_CALL( this, game ) {

					game->CheckRW( GSE_CALL );

					N_EXPECT_ARGS( 1 );
					N_GETVALUE( isready, 0, Bool );

					if ( isready ) {
						m_slot->SetPlayerFlag( ::game::backend::slot::PF_READY );
					}
					else {
						m_slot->UnsetPlayerFlag( ::game::backend::slot::PF_READY );
					}

					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"get_faction",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 0 );
					return m_faction
						? m_faction->Wrap( GSE_CALL )
						: VALUE( gse::value::Undefined );
				} )
			},
			{
				"set_faction_by_id",
				NATIVE_CALL( this, game ) {

					game->CheckRW( GSE_CALL );

					N_EXPECT_ARGS( 1 );
					N_GETVALUE( faction_id, 0, String );
					if ( game->IsStarted() ) {
						GSE_ERROR( gse::EC.GAME_ERROR, "Can't change faction after game is started" );
					}
					auto* faction = game->GetFaction( faction_id );
					if ( !faction ) {
						GSE_ERROR( gse::EC.GAME_ERROR, "Faction not found: " + faction_id );
					}
					m_faction = faction;

					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"unset_faction",
				NATIVE_CALL( this, game ) {

					game->CheckRW( GSE_CALL );

					N_EXPECT_ARGS( 0 );
					m_faction = nullptr;

					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"is_master",
				VALUE( gse::value::Bool, , m_role == PR_SINGLE || m_role == PR_HOST )
			},
			{
				"add_production_choice",
				NATIVE_CALL( this, game ) {

					game->CheckRW( GSE_CALL );

					N_EXPECT_ARGS( 2 );
					N_GETVALUE( id, 0, String );
					N_GETVALUE( data, 1, Object );

					AddProductionChoice( GSE_CALL, id, data );

					return VALUE( gse::value::Undefined );
				} )
			},
			{
				"remove_production_choice",
				NATIVE_CALL( this, game ) {

					game->CheckRW( GSE_CALL );

					return VALUE( gse::value::Undefined );
				} )
			},
		};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Player )

const types::Buffer Player::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_name );
	buf.WriteInt( m_role );
	buf.WriteBool( m_faction != nullptr );
	if ( m_faction ) {
		buf.WriteString( m_faction->Serialize().ToString() );
	}
	buf.WriteString( m_difficulty_level );

	buf.WriteInt( m_production_choices.size() );
	for ( const auto& it : m_production_choices ) {
		buf.WriteString( it.first );
		it.second->Serialize( buf );
	}

	return buf;
}

void Player::Deserialize( types::Buffer buf ) {

	m_name = buf.ReadString();
	m_role = (role_t)buf.ReadInt();
	m_faction = {};
	if ( buf.ReadBool() ) {
		m_faction = new faction::Faction();
		m_faction->Deserialize( buf.ReadString() );
	}
	m_difficulty_level = buf.ReadString();

	ASSERT( m_production_choices.empty(), "production choices not empty" );
	const size_t sz = buf.ReadInt();
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto id = buf.ReadString();
		m_production_choices.insert({ id, base::production::Production::Deserialize( buf ) });
	}

}

WRAPIMPL_SERIALIZE( Player )
	buf->WriteInt( obj->m_slotnum );
}

WRAPIMPL_DESERIALIZE( Player )
	const auto slot_num = buf->ReadInt();
	const auto& player = game->GetState()->m_slots->GetSlot( slot_num ).GetPlayer();
	return player->Wrap( GSE_CALL );
}

}
}
