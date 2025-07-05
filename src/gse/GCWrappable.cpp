#include "GCWrappable.h"

#include "gse/value/Object.h"
#include "gc/Space.h"

namespace gse {

GCWrappable::GCWrappable( gc::Space* const gc_space )
	: gc::Object( gc_space ) {}

GCWrappable::~GCWrappable() {}

const GCWrappable::callback_id_t GCWrappable::On( GSE_CALLABLE, const std::string& event, gse::Value* const callback ) {
	Persist( callback );
	return Wrappable::On( GSE_CALL, event, callback );
}

void GCWrappable::Off( GSE_CALLABLE, const std::string& event, const callback_id_t callback_id ) {
	const auto& it1 = m_callbacks.find( event );
	if ( it1 != m_callbacks.end() ) {
		if ( callback_id ) {
			// delete one handler
			const auto& it2 = it1->second.find( callback_id );
			if ( it2 != it1->second.end() ) {
				Unpersist( it2->second.callable );
			}
		}
		else {
			// delete all handlers
			for ( const auto& it2 : it1->second ) {
				Unpersist( it2.second.callable );
			}
		}
	}
	Wrappable::Off( GSE_CALL, event, callback_id );
}

const bool GCWrappable::HasHandlers( const std::string& event ) {
	return Wrappable::HasHandlers( event );
}

void GCWrappable::GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );
	gse::Wrappable::GetReachableObjects( reachable_objects );
}

}
