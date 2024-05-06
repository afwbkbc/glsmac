#include "Binding.h"

#include "gse/type/Array.h"
#include "gse/type/Object.h"

#include "game/event/RequestTileLocks.h"

namespace game {
namespace bindings {

BINDING_IMPL( tiles ) {
	const gse::type::Object::properties_t properties = {
		{
			"lock",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( tiles, 0, Array );
				N_PERSIST_CALLABLE( on_complete, 1 );
				map::Tile::tile_positions_t tile_positions = {};
				tile_positions.reserve( tiles.size() );
				for ( const auto& tileobj : tiles ) {
					N_UNWRAP( tile, tileobj, map::Tile );
					tile_positions.push_back( tile->coord );
				}

				GAME->SendTileLockRequest( tile_positions, [ this, on_complete, tile_positions, ctx, call_si ]() {
					on_complete->Run( ctx, call_si, {
						VALUE( gse::callable::Native, [ this, tile_positions ](
							gse::Context* ctx,
							const gse::si_t& call_si,
							const gse::type::Callable::function_arguments_t& arguments
						) -> gse::Value {
							GAME->SendTileUnlockRequest( tile_positions );
							return VALUE( gse::type::Undefined );
						}),
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
