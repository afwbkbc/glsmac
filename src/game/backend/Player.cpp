#include "Player.h"

#include "game/backend/faction/Faction.h"
#include "game/backend/slot/Slot.h"

#include "gse/value/String.h"
#include "gse/value/Bool.h"

#include "engine/Engine.h"
#include "Game.h"

namespace game {
namespace backend {

Player::Player( types::Buffer buf ) {
	Player::Unserialize( buf );
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
	m_faction = {};
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
						? m_faction->Wrap( GSE_CALL, gc_space )
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

	return buf;
}

void Player::Unserialize( types::Buffer buf ) {

	m_name = buf.ReadString();
	m_role = (role_t)buf.ReadInt();
	m_faction = {};
	if ( buf.ReadBool() ) {
		m_faction = new faction::Faction();
		m_faction->Unserialize( buf.ReadString() );
	}
	m_difficulty_level = buf.ReadString();

}

}
}
