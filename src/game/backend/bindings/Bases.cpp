#include "Binding.h"

#include "Bindings.h"

#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Undefined.h"
#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "game/backend/Game.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/event/SpawnBase.h"

namespace game {
namespace backend {
namespace bindings {

BINDING_IMPL( bases ) {
	const gse::type::object_properties_t properties = {
		{
			"spawn",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 3 );
				N_GETVALUE_UNWRAP( owner, 0, slot::Slot );
				N_GETVALUE_UNWRAP( tile, 1, map::tile::Tile );

				N_GETVALUE( info, 2, Object );
				N_GETPROP_OPT( std::string, name, info, "name", String, "" );
				N_GETPROP_OPT( size_t, population, info, "population", Int, 1 );

				auto* game = GAME;
				game->AddEvent( new event::SpawnBase(
					game->GetSlotNum(),
					owner->GetIndex(),
					tile->coord.x,
					tile->coord.y,
					base::BaseData( name, population )
				) );
				return VALUE( gse::type::Undefined );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
}
