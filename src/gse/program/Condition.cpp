#include "Condition.h"

#include "Expression.h"

namespace gse {
namespace program {

Condition::Condition( const si_t& si, const condition_type_t type )
	: Element( si, ET_CONDITION )
	, type( type ) {}

}
}
