#include "ArrayRef.h"

#include "Array.h"

namespace gse {
namespace value {

void ArrayRef::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	GC_DEBUG_BEGIN( "ArrayRef" );

	GC_DEBUG( "this", this );
	reachable_objects.insert( this );

	GC_DEBUG_BEGIN( "array" );
	GC_REACHABLE( array );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

}
}
