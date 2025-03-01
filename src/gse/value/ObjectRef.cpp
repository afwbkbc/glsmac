#include "ObjectRef.h"

#include "Object.h"

namespace gse {
namespace value {

void ObjectRef::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	GC_DEBUG_BEGIN( "ObjectRef" );

	GC_DEBUG( "this", this );
	reachable_objects.insert( this );

	GC_DEBUG_BEGIN( "object" );
	GC_REACHABLE( object );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

}
}
