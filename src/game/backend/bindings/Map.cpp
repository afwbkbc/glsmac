#include "Binding.h"

#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Object.h"
#include "gse/type/Int.h"
#include "gse/type/Undefined.h"
#include "game/backend/Game.h"
#include "Bindings.h"
#include "game/backend/map/Map.h"

namespace game {
namespace backend {
namespace bindings {

BINDING_IMPL( map ) {
	const gse::type::object_properties_t properties = {
		{
			"get_width",
			NATIVE_CALL( this ) {
				const auto* m = GAME->GetMap();
				return VALUE( gse::type::Int, m->GetWidth() );
			})
		},
		{
			"get_height",
			NATIVE_CALL( this ) {
				const auto* m = GAME->GetMap();
				return VALUE( gse::type::Int, m->GetHeight() );
			})
		},
		{
			"get_tile",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( x, 0, Int );
				N_GETVALUE( y, 1, Int );
				const auto* m = GAME->GetMap();
				const auto w = m->GetWidth();
				const auto h = m->GetHeight();
				if ( x >= w ) {
					ERROR( gse::EC.INVALID_CALL, "X coordinate exceeds map width ( " + std::to_string( x ) + " >= " + std::to_string( w ) + " )" );
				}
				if ( y >= h ) {
					ERROR( gse::EC.INVALID_CALL, "Y coordinate exceeds map height ( " + std::to_string( y ) + " >= " + std::to_string( h ) + " )" );
				}
				if ( x < 0 ) {
					ERROR( gse::EC.INVALID_CALL, "X coordinate can't be negative ( " + std::to_string( x ) + " < 0" );
				}
				if ( y < 0 ) {
					ERROR( gse::EC.INVALID_CALL, "Y coordinate can't be negative ( " + std::to_string( y ) + " < 0" );
				}
				if ( x % 2 != y % 2 ) {
					ERROR( gse::EC.INVALID_CALL, "X and Y oddity differs ( " + std::to_string( x ) + " % 2 != " + std::to_string( y ) + " % 2 )" );
				}
				return m->GetTile( x, y )->Wrap();
			}),
		}
	};

	return VALUE( gse::type::Object, properties );
}

}
}
}
