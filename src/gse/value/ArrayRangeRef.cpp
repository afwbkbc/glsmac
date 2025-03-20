#include "ArrayRangeRef.h"

#include "Array.h"

namespace gse {
namespace value {

void ArrayRangeRef::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	Value::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "ArrayRangeRef" );

	GC_DEBUG_BEGIN( "array" );
	GC_REACHABLE( array );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string ArrayRangeRef::ToString() {
	std::lock_guard guard( m_gc_mutex );
	return "gse::value::ArrayRangeRef( " + array->ToString() + ":" + ( from.has_value()
		? std::to_string( from.value() )
		: ""
	) + ":" + ( to.has_value()
		? std::to_string( to.value() )
		: ""
	) + " )";
}
#endif

}
}
