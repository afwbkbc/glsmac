#include "DifficultyLevel.h"

namespace game {
namespace rules {

DifficultyLevel::DifficultyLevel()
{
	//
}

DifficultyLevel::DifficultyLevel( const std::string& name, const int difficulty )
	: m_name( name )
	, m_difficulty( difficulty )
{
	//
}

const types::Buffer DifficultyLevel::Serialize() const {
	types::Buffer buf;
	
	buf.WriteString( m_name );
	buf.WriteInt( m_difficulty );
	
	return buf;
}

void DifficultyLevel::Unserialize( types::Buffer buf ) {
	
	m_name = buf.ReadString();
	m_difficulty = buf.ReadInt();
	
}

}
}
