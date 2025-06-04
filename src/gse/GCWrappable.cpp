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
	Unpersist( m_callbacks.at( event ).at( callback_id ).callable );
	Wrappable::Off( GSE_CALL, event, callback_id );
}

const bool GCWrappable::HasHandlers( const std::string& event ) {
	return Wrappable::HasHandlers( event );
}

Value* const GCWrappable::Trigger( GSE_CALLABLE, const std::string& event, const f_args_t& f_args, const std::optional< Value::type_t > expected_return_type ) {
	return Wrappable::Trigger( GSE_CALL, event, f_args, expected_return_type );
}

void GCWrappable::GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );
	gse::Wrappable::GetReachableObjects( reachable_objects );
}

}
