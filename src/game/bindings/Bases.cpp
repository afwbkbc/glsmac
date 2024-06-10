#include "Binding.h"

#include "game/bindings/Bindings.h"

#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Undefined.h"
#include "gse/type/Object.h"
#include "game/Game.h"
#include "game/slot/Slot.h"
#include "game/map/tile/Tile.h"
#include "game/event/SpawnBase.h"

namespace game {
namespace bindings {

BINDING_IMPL( bases ) {
	const gse::type::object_properties_t properties = {
		{
			"spawn",
			NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 2 );
			N_GETVALUE_UNWRAP( owner, 0, slot::Slot );
			N_GETVALUE_UNWRAP( tile, 1, map::tile::Tile );
			auto* game = GAME;
			game->AddEvent( new event::SpawnBase(
				game->GetSlotNum(),
				owner->GetIndex(),
				tile->coord.x,
				tile->coord.y
			) );
			return VALUE( gse::type::Undefined );
		})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
