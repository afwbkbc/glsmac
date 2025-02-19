#include "DefineResource.h"

#include "game/backend/Game.h"

#include "game/backend/resource/ResourceManager.h"
#include "game/backend/resource/Resource.h"
#include "gse/value/Undefined.h"

namespace game {
namespace backend {
namespace event {

DefineResource::DefineResource( const size_t initiator_slot, resource::Resource* resource )
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

gse::Value* const DefineResource::Apply( Game* game ) const {
	game->GetRM()->DefineResource( m_resource );
	return VALUE( gse::value::Undefined );
}

TS_BEGIN( DefineResource )
		TS_FUNC_BEGIN( "DefineResource" ) +
			TS_FUNC_ARG( "resource", TS_OF( m_resource ) ) +
		TS_FUNC_END()
TS_END()

void DefineResource::Serialize( types::Buffer& buf, const DefineResource* event ) {
	buf.WriteString( resource::Resource::Serialize( event->m_resource ).ToString() );
}

DefineResource* DefineResource::Unserialize( types::Buffer& buf, const size_t initiator_slot ) {
	auto b = types::Buffer( buf.ReadString() );
	return new DefineResource( initiator_slot, resource::Resource::Unserialize( b ) );
}

}
}
}
