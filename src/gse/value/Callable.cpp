#include "Callable.h"

#include "gse/context/Context.h"

namespace gse {
namespace value {

Callable::Callable( gc::Space* const gc_space, context::Context* ctx )
	: Value( gc_space, GetType() )
	, m_ctx( ctx ) {
	ASSERT_NOLOG( ctx, "callable ctx is null" );
}

void Callable::GetReachableObjects( std::unordered_set< gc::Object* >& active_objects ) {
	std::lock_guard< std::mutex > guard( m_gc_mutex );

	// callable is reachable
	active_objects.insert( this );

	// owner context is reachable
	if ( active_objects.find( m_ctx ) == active_objects.end() ) {
		m_ctx->CollectWithDependencies( active_objects );
	}

}

}
}
