#include "Faction.h"

namespace game {
namespace rules {

Faction::Faction()
{
	//
}

Faction::Faction( const std::string& name, const types::Color& color )
	: m_name( name )
	, m_color( color )
{
	//
}

const types::Buffer Faction::Serialize() const {
	types::Buffer buf;
	
	buf.WriteString( m_name );
	buf.WriteColor( m_color );
	
	return buf;
}

void Faction::Unserialize( types::Buffer buf ) {
	
	m_name = buf.ReadString();
	m_color = buf.ReadColor();
	
}

}
}
