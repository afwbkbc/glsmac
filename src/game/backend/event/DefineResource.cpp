#include "DefineResource.h"

#include "game/backend/Game.h"

#include "game/backend/Resource.h"
#include "gse/type/Undefined.h"

namespace game {
namespace backend {
namespace event {

DefineResource::DefineResource( const size_t initiator_slot, Resource* resource )
	: Event( initiator_slot, ET_RESOURCE_DEFINE )
	, m_resource( resource ) {
	//
}

const std::string* DefineResource::Validate( Game* game ) const {
	if ( m_initiator_slot != 0 ) {
		return Error( "Only master is allowed to define resources" );
	}
	return Ok();
}

const gse::Value DefineResource::Apply( Game* game ) const {
	game->DefineResource( m_resource );
	return VALUE( gse::type::Undefined );
}

TS_BEGIN( DefineResource )
		TS_FUNC_BEGIN( "DefineResource" ) +
			TS_FUNC_ARG( "resource", TS_OF( m_resource ) ) +
		TS_FUNC_END()
TS_END()

void DefineResource::Serialize( types::Buffer& buf, const DefineResource* event ) {
	buf.WriteString( Resource::Serialize( event->m_resource ).ToString() );
}

DefineResource* DefineResource::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	auto b = types::Buffer( buf.ReadString() );
	return new DefineResource( initiator_slot, Resource::Unserialize( b ) );
}

}
}
}
