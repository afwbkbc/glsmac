#include "Player.h"

using namespace types;

namespace game {

Player::Player() {
	//
}

Player::Player( const std::string& name, const role_t role, const rules::Faction& faction )
	: m_name( name )
	, m_role( role )
	, m_faction( faction )
{
	//
}

const bool Player::IsInitialized() const {
	return m_is_initialized;
}

const std::string& Player::GetPlayerName() const {
	return m_name;
}

void Player::SetFaction( const rules::Faction& faction ) {
	// TODO: validate?
	m_faction = faction;
}

const rules::Faction& Player::GetFaction() const {
	return m_faction;
}

const Buffer Player::Serialize() const {
	Buffer buf;
	
	buf.WriteString( m_name );
	buf.WriteInt( m_role );
	
	return buf;
}

void Player::Unserialize( Buffer buf ) {
	
	m_name = buf.ReadString();
	m_role = (role_t)buf.ReadInt();
	
}


}
