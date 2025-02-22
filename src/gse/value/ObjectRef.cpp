#include "ObjectRef.h"

#include "Object.h"

namespace gse {
namespace value {

void ObjectRef::GetReachableObjects( std::unordered_set< gc::Object* >& active_objects ) {

	// ref is reachable
	active_objects.insert( this );

	// target object is reachable
	if ( active_objects.find( object ) == active_objects.end() ) {
		object->GetReachableObjects( active_objects );
	}

}

}
}
