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
	GC_DEBUG_BEGIN( "Callable" );

	// callable is reachable
	GC_DEBUG( "this", this );
	active_objects.insert( this );

	// owner context is reachable
	GC_DEBUG_BEGIN( "owner_context" );
	if ( active_objects.find( m_ctx ) == active_objects.end() ) {
		m_ctx->CollectWithDependencies( active_objects );
	}
	else {
		GC_DEBUG( "ref", m_ctx );
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

}
}
