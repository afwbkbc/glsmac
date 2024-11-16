#include "Binding.h"

#include "Bindings.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Object.h"
#include "gse/type/Undefined.h"

namespace game {
namespace backend {
namespace bindings {

BINDING_IMPL( on ) {

#define ON( _property, _cbtype ) { _property, CALLBACK( Bindings::_cbtype ) }
	const gse::type::object_properties_t properties = {
	};
#undef ON
	return VALUE( gse::type::Object, properties );
}

}
}
}
