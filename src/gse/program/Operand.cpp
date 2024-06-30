#include "Operand.h"

namespace gse {
namespace program {

Operand::Operand( const si_t& si, const operand_type_t type )
	: Element( si, ET_OPERAND )
	, type( type ) {}

}
}
