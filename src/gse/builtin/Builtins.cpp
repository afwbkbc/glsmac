#include "Builtins.h"

#include "gse/type/Null.h"

namespace gse {
namespace builtin {

Value Builtins::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {

	return VALUE( type::Null );
}

}
}
