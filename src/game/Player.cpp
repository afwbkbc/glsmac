#include "Player.h"

using namespace types;

namespace game {

Player::Player() {
	//
}

Player::Player( const std::string& name, const role_t role )
	: m_name( name )
	, m_role( role )
{
	//
}

const bool Player::IsInitialized() const {
	return m_is_initialized;
}

const std::string& Player::GetName() const {
	return m_name;
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
