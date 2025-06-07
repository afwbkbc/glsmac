#pragma once

#include <unordered_set>
#include <unordered_map>
#include <map>
#include <optional>

#include "Wrappable.h"
#include "gc/Object.h"

#include "value/Types.h"
#include "value/Int.h"
#include "value/String.h"
#include "value/Undefined.h"
#include "value/Object.h"
#include "callable/Native.h"
#include "Exception.h"
#include "Value.h"

#define _ARGS_F( ... ) [ __VA_ARGS__ ]( GSE_CALLABLE, gse::value::object_properties_t& out_args )
#define ARGS_F( ... ) _ARGS_F( __VA_ARGS__ ) { out_args =
#define ARGS( _args ) _ARGS_F( &_args ) { out_args = _args; }
#define ARGS_GSE_CALLABLE &ctx, gc_space, &si, &ep

namespace gse {

namespace value {
class Object;
}

class GCWrappable : public Wrappable, public gc::Object {
public:
	GCWrappable( gc::Space* const gc_space );
	virtual ~GCWrappable();
	
	const callback_id_t On( GSE_CALLABLE, const std::string& event, Value* const callback ) override;
	void Off( GSE_CALLABLE, const std::string& event, const callback_id_t callback_id ) override;
	const bool HasHandlers( const std::string& event ) override;
	
	virtual void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) override;
	
};

}
