#include "Production.h"

#include "Facility.h"
#include "Custom.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Production::Production( const std::string& id, const production_type_t type, const std::string& hint, const std::string& intent )
	: m_id( id )
	, m_type( type )
	, m_hint( hint )
	, m_intent( intent ) {}

void Production::Serialize( types::Buffer& buf ) const {
	buf.WriteString( m_id );
	buf.WriteInt( m_type );
	buf.WriteString( m_hint );
	buf.WriteString( m_intent );
	SerializeImpl( buf );
}

const Production* const Production::Deserialize( types::Buffer& buf ) {
	const auto id = buf.ReadString();
	const auto type = (production_type_t)buf.ReadInt();
	const auto hint = buf.ReadString();
	const auto intent = buf.ReadString();
	switch ( type ) {
		case PT_FACILITY:
			return Facility::DeserializeImpl( buf, id, hint, intent );
		case PT_CUSTOM:
			return Custom::DeserializeImpl( buf, id, hint, intent );
		default:
			THROW( "unknown production choice type: " + std::to_string( type ) );
	}
}

}
}
}
}
