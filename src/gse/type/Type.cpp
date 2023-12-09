#include "Type.h"

#include "base/Base.h"

#include "Null.h"
#include "Bool.h"
#include "Object.h"

namespace gse {
namespace type {

const std::string Type::GetTypeString( const type_t type ) {
	switch ( type ) {
		case T_UNDEFINED:
			return "undefined";
		case T_NULL:
			return "null";
		case T_BOOL:
			return "bool";
		case T_INT:
			return "int";
		case T_STRING:
			return "string";
		case T_OBJECT:
			return "object";
		case T_CALLABLE:
			return "callable";
		default:
			return "unknown";
	}
}

}
}
