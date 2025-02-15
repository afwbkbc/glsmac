#include "Value.h"

#include "common/Common.h"

#include "type/Undefined.h"
#include "type/Null.h"
#include "type/Bool.h"
#include "type/Int.h"
#include "type/Float.h"
#include "type/String.h"
#include "type/Array.h"
#include "type/Object.h"
#include "type/ArrayRef.h"
#include "type/ObjectRef.h"

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

const std::string& Value::GetTypeString() const {
	return m_data.get()->GetTypeString( m_data.get()->type );
}

const std::string Value::ToString() const {
	return m_data->ToString();
}

const std::string Value::Dump() const {
	return m_data->Dump();
}

const Value Value::Clone() const {
	return New( m_data.get() );
}

#define OP( _op ) \
const bool Value::operator _op( const Value& other ) const { \
    return *m_data->Deref() _op *other.m_data->Deref(); \
}
OP( == )
OP( != )
OP( < )
OP( > )
OP( <= )
OP( >= )
#undef OP

void Value::Serialize( types::Buffer* buf, const Value& value ) {
	type::Type::Serialize( buf, value.Get() );
}

Value Value::Unserialize( types::Buffer* buf ) {
	return type::Type::Unserialize( buf );
}

const Value Value::New( const type::Type* type ) const {
	switch ( type->type ) {
		case type::Type::T_UNDEFINED:
			return VALUE( type::Undefined );
		case type::Type::T_NULL:
			return VALUE( type::Null );
		case type::Type::T_BOOL:
			return VALUE( type::Bool, ( (type::Bool*)type )->value );
		case type::Type::T_INT:
			return VALUE( type::Int, ( (type::Int*)type )->value );
		case type::Type::T_FLOAT:
			return VALUE( type::Float, ( (type::Float*)type )->value );
		case type::Type::T_STRING:
			return VALUE( type::String, ( (type::String*)type )->value );
		case type::Type::T_ARRAY: {
			type::array_elements_t elements = {};
			const auto* arr = (type::Array*)type;
			for ( const auto& it : arr->value ) {
				elements.push_back( it.Clone() );
			}
			return VALUE( type::Array, elements );
		}
		case type::Type::T_OBJECT: {
			type::object_properties_t properties = {};
			const auto* obj = (type::Object*)type;
			for ( const auto& it : obj->value ) {
				properties.insert_or_assign( it.first, it.second.Clone() );
			}
			return VALUE( type::Object, nullptr, properties, obj->object_class, obj->wrapobj, obj->wrapsetter );
		}
		case type::Type::T_CALLABLE:
			return *this;
		case type::Type::T_ARRAYREF:
		case type::Type::T_ARRAYRANGEREF:
		case type::Type::T_OBJECTREF:
		case type::Type::T_VALUEREF: {
			// no need to keep ref to old value if it's a copy
			return New( type->Deref() );
		}
		case type::Type::T_RANGE:
			THROW( "ranges are not supposed to be cloned" );
		case type::Type::T_LOOPCONTROL:
			THROW( "loop controls are not supposed to be cloned" );
		default: {
			THROW( "unsupported value type: " + ToString() );
		}
	}
}

}
