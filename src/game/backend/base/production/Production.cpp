#include "Production.h"

#include "Facility.h"
#include "Custom.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Production::Production( const production_type_t type )
	: m_type( type ) {}

void Production::Serialize( types::Buffer& buf ) const {
	buf.WriteInt( m_type );
	SerializeImpl( buf );
}

const Production* const Production::Deserialize( types::Buffer& buf ) {
	const auto type = (production_type_t)buf.ReadInt();
	switch ( type ) {
		case PT_FACILITY:
			return Facility::DeserializeImpl( buf );
		case PT_CUSTOM:
			return Custom::DeserializeImpl( buf );
		default:
			THROW( "unknown production choice type: " + std::to_string( type ) );
	}
}

}
}
}
}
