#include "Binding.h"

#include "gse/type/String.h"

namespace game {
namespace bindings {

BINDING_IMPL( message ) {
	return NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 1 );
		N_GETVALUE( text, 0, String );
		m_game->Message( text );
		return VALUE( gse::type::Undefined );
	});
}

}
}
