#include "ValueRef.h"

namespace gse {
namespace value {

void ValueRef::GetReachableObjects( std::unordered_set< gc::Object* >& active_objects ) {

	// ref is reachable
	active_objects.insert( this );

	// target value is reachable
	if ( active_objects.find( target ) == active_objects.end() ) {
		target->GetReachableObjects( active_objects );
	}
}

}
}
