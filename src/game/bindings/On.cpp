#include "Binding.h"

#include "gse/type/Object.h"

namespace game {
namespace bindings {

BINDING_IMPL( on ) {
	const gse::type::Object::properties_t properties = {
		{
			"start",
			CALLBACK( Bindings::CS_ON_START )
		},
		{
			"spawn_unit",
			CALLBACK( Bindings::CS_ON_SPAWN_UNIT )
		},
	};
	return VALUE( gse::type::Object, properties );
}

}
}
