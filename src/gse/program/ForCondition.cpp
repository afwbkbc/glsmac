#include "ForCondition.h"

namespace gse {
namespace program {

ForCondition::ForCondition( const si_t& si, const for_condition_type_t type )
	: Condition( si, CT_SIMPLE )
	, for_type( type ) {}

}
}
