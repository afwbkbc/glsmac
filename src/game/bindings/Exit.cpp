#include "Binding.h"

#include "gse/type/String.h"

namespace game {
namespace bindings {

BINDING_IMPL( exit ) {
	return NATIVE_CALL( this ) {
		N_EXPECT_ARGS_MIN_MAX( 0, 1 );
		if ( arguments.size() == 1 ) {
			N_GETVALUE( reason, 0, String );
			GAME->Quit( reason );
		}
		else {
			GAME->Quit( "Script exited" );
		}
		return VALUE( gse::type::Undefined );
	});
}

}
}
