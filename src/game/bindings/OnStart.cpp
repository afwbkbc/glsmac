#include "Binding.h"

namespace game {
namespace bindings {

BINDING_IMPL( on_start ) {
	return CALLBACK( Bindings::CS_ONSTART );
}

}
}
