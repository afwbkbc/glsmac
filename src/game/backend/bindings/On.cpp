#include "Binding.h"

#include "Bindings.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Object.h"
#include "gse/type/Undefined.h"

namespace game {
namespace backend {
namespace bindings {

BINDING_IMPL( on ) {

#define ON( _property, _cbtype ) { _property, CALLBACK( Bindings::_cbtype ) }
	const gse::type::object_properties_t properties = {
		ON( "configure", CS_ON_CONFIGURE ),
		ON( "start", CS_ON_START ),
		ON( "turn", CS_ON_TURN ),
		ON( "get_tile_yields", CS_ON_GET_TILE_YIELDS ),
		ON( "unit_spawn", CS_ON_UNIT_SPAWN ),
		ON( "unit_despawn", CS_ON_UNIT_DESPAWN ),
		ON( "unit_move_validate", CS_ON_UNIT_MOVE_VALIDATE ),
		ON( "unit_move_resolve", CS_ON_UNIT_MOVE_RESOLVE ),
		ON( "unit_move_apply", CS_ON_UNIT_MOVE_APPLY ),
		ON( "unit_attack_validate", CS_ON_UNIT_ATTACK_VALIDATE ),
		ON( "unit_attack_resolve", CS_ON_UNIT_ATTACK_RESOLVE ),
		ON( "unit_attack_apply", CS_ON_UNIT_ATTACK_APPLY ),
		ON( "unit_turn", CS_ON_UNIT_TURN ),
	};
#undef ON
	return VALUE( gse::type::Object, properties );
}

}
}
}
