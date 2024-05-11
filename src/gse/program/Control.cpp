#include "Control.h"

namespace gse {
namespace program {

Control::Control( const si_t& si, const control_type_t control_type )
	: Element( si, ET_CONDITIONAL )
	, control_type( control_type ) {}

}
}
