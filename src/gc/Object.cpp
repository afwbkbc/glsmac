#include "Object.h"

#include "Space.h"

namespace gc {

Object::Object( gc::Space* const gc_space ) {
	if ( gc_space ) {
		gc_space->Add( this );
	}
}

void Object::GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) {
	GC_DEBUG_BEGIN( "gc::Object" );

	GC_DEBUG( "this", this );
	reachable_objects.insert( this );

	GC_DEBUG_END();
}

}
