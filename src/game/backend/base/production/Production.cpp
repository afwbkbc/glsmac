#include "Production.h"

#include "Facility.h"
#include "Custom.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Production::Production( const production_type_t type, const std::string& hint )
	: m_type( type )
	, m_hint( hint ) {}

void Production::Serialize( types::Buffer& buf ) const {
	buf.WriteInt( m_type );
	buf.WriteString( m_hint );
	SerializeImpl( buf );
}

const Production* const Production::Deserialize( types::Buffer& buf ) {
	const auto type = (production_type_t)buf.ReadInt();
	const std::string hint = buf.ReadString();
	switch ( type ) {
		case PT_FACILITY:
			return Facility::DeserializeImpl( buf, hint );
		case PT_CUSTOM:
			return Custom::DeserializeImpl( buf, hint );
		default:
			THROW( "unknown production choice type: " + std::to_string( type ) );
	}
}

}
}
}
}
