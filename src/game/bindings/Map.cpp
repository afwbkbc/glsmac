#include "Binding.h"

#include "gse/type/Object.h"
#include "gse/type/Int.h"

namespace game {
namespace bindings {

BINDING_IMPL( map ) {
	const auto* m = m_game->GetMap();
	const auto w = m->GetWidth();
	const auto h = m->GetHeight();
	const gse::type::Object::properties_t properties = {
		{
			"width",
			VALUE( gse::type::Int, w ),
		},
		{
			"height",
			VALUE( gse::type::Int, h ),
		},
		{
			"get_tile",
			NATIVE_CALL( m, w, h ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( x, 0, Int );
				N_GETVALUE( y, 1, Int );
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
