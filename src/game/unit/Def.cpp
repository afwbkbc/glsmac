#include "Def.h"

#include "StaticDef.h"

#include "gse/type/Object.h"
#include "gse/type/String.h"

namespace game {
namespace unit {

Def::Def( const def_type_t type, const std::string& name )
	: m_type( type )
	, m_name( name ) {
	//
}

const types::Buffer Def::Serialize( const Def* def ) {
	types::Buffer buf;
	buf.WriteString( def->m_name );
	buf.WriteInt( def->m_type );
	switch ( def->m_type ) {
		case DT_STATIC: {
			StaticDef::Serialize( buf, (StaticDef*)def );
			break;
		}
		default:
			THROW( "unknown def type on write: " + std::to_string( def->m_type ) );
	}
	return buf;
}

Def* Def::Unserialize( types::Buffer& buf ) {
	const auto name = buf.ReadString();
	const auto type = (def_type_t)buf.ReadInt();
	switch ( type ) {
		case DT_STATIC:
			return StaticDef::Unserialize( buf, name );
		default:
			THROW( "unknown def type on read: " + std::to_string( type ) );
	}
}

const gse::Value Def::ToGSEObject() const {
	const gse::type::Object::properties_t properties = {
		{
			"name",
			VALUE( gse::type::String, m_name )
		},
		{
			"type",
			VALUE( gse::type::String, "static" ) // TODO
		},
	};
	return VALUE( gse::type::Object, properties, gse::type::Object::CLASS_UNITDEF );
}

}
}
