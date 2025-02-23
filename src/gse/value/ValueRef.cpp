#include "ValueRef.h"

namespace gse {
namespace value {

void ValueRef::GetReachableObjects( std::unordered_set< gc::Object* >& active_objects ) {
	GC_DEBUG_BEGIN( "ValueRef" );

	// ref is reachable
	GC_DEBUG( "this", this );
	active_objects.insert( this );

	// target value is reachable
	GC_DEBUG_BEGIN( "target" );
	if ( active_objects.find( target ) == active_objects.end() ) {
		target->GetReachableObjects( active_objects );
	}
	else {
		GC_DEBUG( "ref", target );
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

}
}
