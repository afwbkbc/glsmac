#include "Binding.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Array.h"
#include "gse/type/Object.h"
#include "gse/type/Undefined.h"
#include "game/backend/Game.h"
#include "game/backend/map/tile/TileManager.h"
#include "game/backend/event/RequestTileLocks.h"
#include "game/backend/map/tile/Tile.h"
#include "Bindings.h"

namespace game {
namespace backend {
namespace bindings {

BINDING_IMPL( tiles ) {
	const gse::type::object_properties_t properties = {
		{
			"lock",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( tiles, 0, Array );
				N_PERSIST_CALLABLE( on_complete, 1 );
				map::tile::positions_t tile_positions = {};
				tile_positions.reserve( tiles.size() );
				for ( const auto& tileobj : tiles ) {
					N_UNWRAP( tile, tileobj, map::tile::Tile );
					tile_positions.push_back( tile->coord );
				}

				GAME->GetTM()->SendTileLockRequest( tile_positions, [ this, on_complete, tile_positions, ctx, call_si ]() {
					on_complete->Run( ctx, call_si, {
						VALUE( gse::callable::Native, [ this, tile_positions ](
							gse::context::Context* ctx,
							const gse::si_t& call_si,
							const gse::type::function_arguments_t& arguments
						) -> gse::Value {
							GAME->GetTM()->SendTileUnlockRequest( tile_positions );
							return VALUE( gse::type::Undefined );
						} )
					});
					N_UNPERSIST_CALLABLE( on_complete );
				});
				return VALUE( gse::type::Undefined );
			})
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
}
