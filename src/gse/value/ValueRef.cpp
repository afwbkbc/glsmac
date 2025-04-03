#include "ValueRef.h"

namespace gse {
namespace value {

void ValueRef::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	Value::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "ValueRef" );

	GC_DEBUG_BEGIN( "target" );
	GC_REACHABLE( target );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string ValueRef::ToString() {
	return "gse::value::ValueRef( " + target->ToString() + ")";
}
#endif

}
}
