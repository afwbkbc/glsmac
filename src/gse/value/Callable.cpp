#include "Callable.h"

#include "gse/context/Context.h"

namespace gse {
namespace value {

Callable::Callable( gc::Space* const gc_space, context::Context* ctx )
	: Value( gc_space, GetType() )
	, m_ctx( ctx ) {
	ASSERT_NOLOG( ctx, "callable ctx is null" );
}

void Callable::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	Value::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Callable" );

	GC_DEBUG_BEGIN( "owner_context" );
	GC_REACHABLE( m_ctx );
	GC_DEBUG_END();

	GC_DEBUG_END();
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string Callable::ToString() {
	return "gse::value::Callable()";
}
#endif

}
}
