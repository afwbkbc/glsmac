#include "Player.h"

using namespace types;

namespace game {

// builtin
const rules::Faction Player::RANDOM_FACTION{
	"RANDOM",
	"Random"
};

Player::Player( Buffer buf ) {
	Player::Unserialize( buf );
}

Player::Player(
	const std::string& name,
	const role_t role,
	const rules::Faction& faction,
	const rules::DifficultyLevel& difficulty_level
)
	: m_name( name )
	, m_role( role )
	, m_faction( faction )
	, m_difficulty_level( difficulty_level ) {
	//
}

const std::string& Player::GetPlayerName() const {
	return m_name;
}

const std::string Player::GetFullName() const {
	return GetPlayerName() + " (" + GetFaction().m_name + ")";
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

void Player::SetFaction( const rules::Faction& faction ) {
	// TODO: validate?
	m_faction = faction;
}

const rules::Faction& Player::GetFaction() const {
	return m_faction;
}

void Player::SetDifficultyLevel( const rules::DifficultyLevel& difficulty_level ) {
	// TODO: validate?
	m_difficulty_level = difficulty_level;
}

const rules::DifficultyLevel& Player::GetDifficultyLevel() const {
	return m_difficulty_level;
}

void Player::SetSlot( Slot* slot ) {
	m_slot = slot;
}

Slot* Player::GetSlot() const {
	return m_slot;
}

const Player::role_t Player::GetRole() const {
	return m_role;
}

const bool Player::IsTurnCompleted() const {
	return m_is_turn_completed;
}

void Player::CompleteTurn() {
	ASSERT( !m_is_turn_completed, "turn already completed" );
	m_is_turn_completed = true;
}

void Player::UncompleteTurn() {
	ASSERT( m_is_turn_completed, "turn not completed" );
	m_is_turn_completed = false;
}

const Buffer Player::Serialize() const {
	Buffer buf;

	buf.WriteString( m_name );
	buf.WriteInt( m_role );
	buf.WriteString( m_faction.Serialize().ToString() );
	buf.WriteString( m_difficulty_level.Serialize().ToString() );

	return buf;
}

void Player::Unserialize( Buffer buf ) {

	m_name = buf.ReadString();
	m_role = (role_t)buf.ReadInt();
	m_faction.Unserialize( buf.ReadString() );
	m_difficulty_level.Unserialize( buf.ReadString() );

}

}
