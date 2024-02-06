#include "Binding.h"

namespace game {
namespace bindings {

BINDING_IMPL( exit ) {
	return NATIVE_CALL( this ) {
		N_EXPECT_ARGS( 0 );
		m_game->Quit();
		return VALUE( gse::type::Undefined );
	});
}

}
}
