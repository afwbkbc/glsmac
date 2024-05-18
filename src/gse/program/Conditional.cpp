#include "Conditional.h"

namespace gse {
namespace program {

Conditional::Conditional( const si_t& si, const conditional_type_t conditional_type )
	: Control( si, CT_CONDITIONAL )
	, conditional_type( conditional_type ) {}

}
}
