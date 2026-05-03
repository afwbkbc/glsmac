#include "Ptr.h"

#include "gse/value/Float.h"

namespace gse {
namespace value {

Ptr::Ptr( gc::Space* const gc_space, const value_type_t ptr_type, void* const target )
	: Value( gc_space, GetType() )
	, m_ptr_type( ptr_type )
	, m_target( target ) {}

gse::Value* const Ptr::GetValue() const {
	switch ( m_ptr_type ) {
		case VT_FLOAT: {
			return VALUEEXT( value::Float, m_gc_space, *( (std::atomic< float >*)m_target ) );
		}
		default:
			THROW( "unsupported ptr type: " + Value::GetTypeStringStatic( m_ptr_type ) );
	}
}

}
}
