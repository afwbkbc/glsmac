#include "Binding.h"

#include "gse/type/Object.h"

namespace game {
namespace bindings {

BINDING_IMPL( on ) {

#define ON( _property, _cbtype ) { _property, CALLBACK( Bindings::_cbtype ) }
	const gse::type::Object::properties_t properties = {
		ON( "configure", CS_ON_CONFIGURE ),
		ON( "start", CS_ON_START ),
		ON( "turn", CS_ON_TURN ),
		ON( "unit_spawn", CS_ON_UNIT_SPAWN ),
		ON( "unit_despawn", CS_ON_UNIT_DESPAWN ),
		ON( "unit_move_validate", CS_ON_UNIT_MOVE_VALIDATE ),
		ON( "unit_move_resolve", CS_ON_UNIT_MOVE_RESOLVE ),
		ON( "unit_move_apply", CS_ON_UNIT_MOVE_APPLY ),
		ON( "unit_turn", CS_ON_UNIT_TURN ),
	};
#undef ON
	return VALUE( gse::type::Object, properties );
}

}
}
