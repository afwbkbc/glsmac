#include "Production.h"

#include "Facility.h"
#include "Custom.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Production::Production( const production_type_t type, const std::string& hint, const std::string& intent )
	: m_type( type )
	, m_hint( hint )
	, m_intent( intent ) {}

void Production::Serialize( types::Buffer& buf ) const {
	buf.WriteInt( m_type );
	buf.WriteString( m_hint );
	buf.WriteString( m_intent );
	SerializeImpl( buf );
}

const Production* const Production::Deserialize( types::Buffer& buf ) {
	const auto type = (production_type_t)buf.ReadInt();
	const std::string hint = buf.ReadString();
	const std::string intent = buf.ReadString();
	switch ( type ) {
		case PT_FACILITY:
			return Facility::DeserializeImpl( buf, hint, intent );
		case PT_CUSTOM:
			return Custom::DeserializeImpl( buf, hint, intent );
		default:
			THROW( "unknown production choice type: " + std::to_string( type ) );
	}
}

}
}
}
}
