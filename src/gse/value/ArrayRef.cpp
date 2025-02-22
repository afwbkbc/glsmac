#include "ArrayRef.h"

#include "Array.h"

namespace gse {
namespace value {

void ArrayRef::GetReachableObjects( std::unordered_set< gc::Object* >& active_objects ) {

	// ref is reachable
	active_objects.insert( this );

	// target array is reachable
	if ( active_objects.find( array ) == active_objects.end() ) {
		array->GetReachableObjects( active_objects );
	}

}

}
}
