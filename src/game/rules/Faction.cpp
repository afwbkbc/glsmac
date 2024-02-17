#include "Faction.h"

#include "gse/type/String.h"

namespace game {
namespace rules {

Faction::Faction() {
	//
}

Faction::Faction( const std::string& id, const std::string& name, const types::Color& color )
	: m_id( id )
	, m_name( name )
	, m_color( color ) {
	//
}

const types::Buffer Faction::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_id );
	buf.WriteString( m_name );
	buf.WriteColor( m_color );

	return buf;
}

void Faction::Unserialize( types::Buffer buf ) {

	m_id = buf.ReadString();
	m_name = buf.ReadString();
	m_color = buf.ReadColor();

}

WRAPIMPL_BEGIN( Faction, CLASS_FACTION )
	WRAPIMPL_PROPS {
		{
			"id",
			VALUE( gse::type::String, m_id )
		},
		{
			"name",
			VALUE( gse::type::String, m_name )
		},
		{
			"color",
			m_color.Wrap()
		},
	};
WRAPIMPL_END_PTR( Faction )

UNWRAPIMPL_PTR( Faction )

}
}
