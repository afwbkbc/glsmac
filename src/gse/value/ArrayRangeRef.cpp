#include "ArrayRangeRef.h"

#include "Array.h"

namespace gse {
namespace value {

void ArrayRangeRef::GetReachableObjects( std::unordered_set< gc::Object* >& active_objects ) {
	GC_DEBUG_BEGIN( "ArrayRangeRef" );

	// ref is reachable
	GC_DEBUG( "this", this );
	active_objects.insert( this );

	// target array is reachable
	GC_DEBUG_BEGIN( "array" );
	if ( active_objects.find( array ) == active_objects.end() ) {
		array->GetReachableObjects( active_objects );
	}
	else {
		GC_DEBUG( "ref", array );
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

}
}
