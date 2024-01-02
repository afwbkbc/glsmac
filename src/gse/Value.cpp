#include "Value.h"

#include "base/Base.h"

#include "type/Undefined.h"
#include "type/Null.h"
#include "type/Bool.h"
#include "type/Int.h"
#include "type/Float.h"
#include "type/String.h"
#include "type/Array.h"
#include "type/Object.h"
#include "type/ArrayRef.h"
#include "type/ArrayRangeRef.h"
#include "type/ObjectRef.h"
#include "type/Range.h"

namespace gse {

Value::Value( const std::shared_ptr< type::Type > data )
	: m_data( data ) {
	// nothing
}

Value::~Value() {
	// nothing
}

const type::Type* Value::Get() const {
	return m_data.get();
}

const std::string Value::ToString() const {
	return m_data->ToString();
}

const Value Value::Clone() const {
	switch ( m_data.get()->type ) {
		case type::Type::T_UNDEFINED:
			return VALUE( type::Undefined );
		case type::Type::T_NULL:
			return VALUE( type::Null );
		case type::Type::T_BOOL:
			return VALUE( type::Bool, ( (type::Bool*)m_data.get() )->value );
		case type::Type::T_INT:
			return VALUE( type::Int, ( (type::Int*)m_data.get() )->value );
		case type::Type::T_FLOAT:
			return VALUE( type::Float, ( (type::Float*)m_data.get() )->value );
		case type::Type::T_STRING:
			return VALUE( type::String, ( (type::String*)m_data.get() )->value );
		case type::Type::T_ARRAY: {
			type::Array::elements_t elements = {};
			const auto* arr = (type::Array*)m_data.get();
			for ( const auto& it : arr->value ) {
				elements.push_back( it.Clone() );
			}
			return VALUE( type::Array, elements );
		}
		case type::Type::T_OBJECT: {
			type::Object::properties_t properties = {};
			const auto* obj = (type::Object*)m_data.get();
			for ( const auto& it : obj->value ) {
				properties.insert_or_assign( it.first, it.second.Clone() );
			}
			return VALUE( type::Object, properties );
		}
		case type::Type::T_CALLABLE:
			return *this;
		case type::Type::T_ARRAYREF:
			ASSERT_NOLOG( false, "refs are not supposed to be cloned" );
		case type::Type::T_ARRAYRANGEREF:
			ASSERT_NOLOG( false, "refs are not supposed to be cloned" );
		case type::Type::T_OBJECTREF:
			ASSERT_NOLOG( false, "refs are not supposed to be cloned" );
		case type::Type::T_RANGE:
			ASSERT_NOLOG( false, "ranges are not supposed to be cloned" );
		default: {
			ASSERT_NOLOG( false, "unsupported value type: " + ToString() );
		}
	}
}

#define OP( _op ) \
const bool Value::operator _op( const Value& other ) const { \
    return *m_data _op *other.m_data; \
}
OP( == )
OP( != )
OP( < )
OP( > )
OP( <= )
OP( >= )
#undef OP

}