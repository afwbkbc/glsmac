#include "Binding.h"

#include "game/bindings/Bindings.h"
#include "game/Game.h"
#include "gse/type/String.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Undefined.h"

namespace game {
namespace bindings {

BINDING_IMPL( message ) {
	return NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( text, 0, String );
		GAME->Message( text );
		return VALUE( gse::type::Undefined );
	});
}

}
}
