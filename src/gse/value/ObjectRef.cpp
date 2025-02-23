#include "ObjectRef.h"

#include "Object.h"

namespace gse {
namespace value {

void ObjectRef::GetReachableObjects( std::unordered_set< gc::Object* >& active_objects ) {
	GC_DEBUG_BEGIN( "ObjectRef" );

	// ref is reachable
	GC_DEBUG( "this", this );
	active_objects.insert( this );

	// target object is reachable
	GC_DEBUG_BEGIN( "object" );
	if ( active_objects.find( object ) == active_objects.end() ) {
		object->GetReachableObjects( active_objects );
	}
	else {
		GC_DEBUG( "ref", object );
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

}
}
