#include "Builtins.h"

#include "gse/type/Null.h"

namespace gse {
namespace builtin {

Value Builtins::Run( GSE* gse, const Callable::function_arguments_t& arguments ) {

	return VALUE( type::Null );
}

}
}
