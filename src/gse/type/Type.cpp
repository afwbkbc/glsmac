#include "Type.h"

#include "base/Base.h"

#include "Null.h"
#include "Bool.h"
#include "Object.h"

namespace gse {
namespace type {

Type Type::operator=( const Type& other ) {
	switch ( other.type ) {
		case T_NULL:
			return Null();
		case T_BOOL:
			return Bool( *( (const Bool*)&other ) );
		case T_OBJECT:
			return Object( *( (const Object*)&other ) );
		default:
			ASSERT_NOLOG( "unsupported/unimplemented type: " + std::to_string( other.type ) );
	}
}

}
}