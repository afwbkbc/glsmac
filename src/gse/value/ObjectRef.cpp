#include "ObjectRef.h"

#include "Object.h"

namespace gse {
namespace value {

void ObjectRef::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	Value::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "ObjectRef" );

	GC_DEBUG_BEGIN( "object" );
	GC_REACHABLE( object );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string ObjectRef::ToString() {
	return "gse::value::ObjectRef( " + object->ToString() + ")";
}
#endif

}
}
