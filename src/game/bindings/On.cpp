#include "Binding.h"

#include "gse/type/Object.h"

namespace game {
namespace bindings {

BINDING_IMPL( on ) {

#define ON( _property, _cbtype ) { _property, CALLBACK( Bindings::_cbtype ) }
	const gse::type::Object::properties_t properties = {
		ON( "configure", CS_ON_CONFIGURE ),
		ON( "start", CS_ON_START ),
		ON( "spawn_unit", CS_ON_SPAWN_UNIT ),
		ON( "despawn_unit", CS_ON_DESPAWN_UNIT ),
	};
#undef ON
	return VALUE( gse::type::Object, properties );
}

}
}
