#include "Binding.h"

#include "gse/type/String.h"
#include "gse/type/Int.h"

#include "../event/SpawnUnit.h"

namespace game {
namespace bindings {

BINDING_IMPL( spawn_unit ) {
	return NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 3 );
		N_GETVALUE( name, 0, String );
		N_GETVALUE( pos_x, 1, Int );
		N_GETVALUE( pos_y, 2, Int );
		m_game->AddGameEvent( new event::SpawnUnit( name, pos_x, pos_y ), ctx, call_si );
		return VALUE( gse::type::Undefined );
	});
}

}
}
