#include "ArrayRef.h"

#include "Array.h"

namespace gse {
namespace value {

void ArrayRef::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	Value::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "ArrayRef" );

	GC_DEBUG_BEGIN( "array" );
	GC_REACHABLE( array );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string ArrayRef::ToString() {
	std::lock_guard guard( m_gc_mutex );
	return "gse::value::ArrayRef( " + array->ToString() + ")";
}
#endif

}
}
