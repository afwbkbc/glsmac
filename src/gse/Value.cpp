#include "Value.h"

#include "common/Common.h"

#include "value/Undefined.h"
#include "value/Null.h"
#include "value/Bool.h"
#include "value/Int.h"
#include "value/Float.h"
#include "value/String.h"
#include "value/Array.h"
#include "value/Object.h"
#include "value/ArrayRef.h"
#include "value/ArrayRangeRef.h"
#include "value/ObjectRef.h"
#include "value/ValueRef.h"
#include "value/Range.h"
#include "value/Exception.h"
#include "value/LoopControl.h"

#include "types/Buffer.h"

#include "gc/Space.h"

namespace gse {

static const std::string s_t_undefined = "Undefined";
static const std::string s_t_null = "Null";
static const std::string s_t_bool = "Bool";
static const std::string s_t_int = "Int";
static const std::string s_t_float = "Float";
static const std::string s_t_string = "String";
static const std::string s_t_array = "Array";
static const std::string s_t_object = "Object";
static const std::string s_t_callable = "Callable";
static const std::string s_t_arrayref = "Arrayref";
static const std::string s_t_arrayrangeref = "Arrayrangeref";
static const std::string s_t_objectref = "Objectref";
static const std::string s_t_valueref = "Valueref";
static const std::string s_t_range = "Range";
static const std::string s_t_loopcontrol = "LoopControl";
static const std::string s_t_unknown = "Unknown";

const std::string& Value::GetTypeStringStatic( const type_t type ) {
	switch ( type ) {
		case T_UNDEFINED:
			return s_t_undefined;
		case T_NULL:
			return s_t_null;
		case T_BOOL:
			return s_t_bool;
		case T_INT:
			return s_t_int;
		case T_FLOAT:
			return s_t_float;
		case T_STRING:
			return s_t_string;
		case T_ARRAY:
			return s_t_array;
		case T_OBJECT:
			return s_t_object;
		case T_CALLABLE:
			return s_t_callable;
		case T_ARRAYREF:
			return s_t_arrayref;
		case T_ARRAYRANGEREF:
			return s_t_arrayrangeref;
		case T_OBJECTREF:
			return s_t_objectref;
		case T_VALUEREF:
			return s_t_valueref;
		case T_RANGE:
			return s_t_range;
		case T_LOOPCONTROL:
			return s_t_loopcontrol;
		default:
			return s_t_unknown;
	}
}

const std::string& Value::GetTypeString() const {
	return GetTypeStringStatic( type );
}

const std::string Value::ToString() const {
	switch ( type ) {
		case T_UNDEFINED:
			return "undefined";
		case T_NULL:
			return "null";
		case T_BOOL:
			return ( (value::Bool*)this )->value
				? "true"
				: "false";
		case T_INT:
			return std::to_string( ( (value::Int*)this )->value );
		case T_FLOAT:
			return std::to_string( ( (value::Float*)this )->value );
		case T_STRING:
			return ( (value::String*)this )->value;
		case T_ARRAY: {
			std::string str = "";
			str.append( "[ " );
			bool first = true;
			for ( const auto& it : ( (value::Array*)this )->value ) {
				if ( first ) {
					first = false;
				}
				else {
					str.append( ", " );
				}
				str.append( it->ToString() );
			}
			str.append( " ]" );
			return str;
		}
		case T_OBJECT: {
			const auto* obj = (value::Object*)this;
			std::string str = "";
			str.append( obj->object_class + "{ " );
			bool first = true;
			for ( const auto& it : obj->value ) {
				if ( first ) {
					first = false;
				}
				else {
					str.append( ", " );
				}
				str.append( it.first + ": " + it.second->ToString() );
			}
			str.append( " }" );
			return str;
		}
		case T_CALLABLE:
			return "()";
		case T_ARRAYREF: {
			const auto* that = (value::ArrayRef*)this;
			return that->array->Get( that->index )->ToString();
		}
		case T_ARRAYRANGEREF: {
			const auto* that = (value::ArrayRangeRef*)this;
			return that->array->GetSubArray( that->from, that->to )->ToString();
		}
		case T_OBJECTREF: {
			const auto* that = (value::ObjectRef*)this;
			return that->object->Get( that->key )->ToString();
		}
		case T_VALUEREF: {
			const auto* that = (value::ValueRef*)this;
			return that->target->ToString();
		}
		case T_RANGE: {
			const auto* that = (value::Range*)this;
			return "[" + (
				that->from
					? std::to_string( *that->from )
					: ""
			) + ":" + (
				that->to
					? std::to_string( *that->to )
					: ""
			) + "]";
		}
		case T_LOOPCONTROL: {
			switch ( ( (value::LoopControl*)this )->value ) {
				case program::LCT_BREAK:
					return "break";
				case program::LCT_CONTINUE:
					return "continue";
				default:
					THROW( "unexpected loop control type: " + std::to_string( ( (value::LoopControl*)this )->value ) );
			}
		}
		default:
			THROW( "unknown is not intended to be printed" );
	}
}

const std::string Value::Dump() const {
	switch ( type ) {
		case T_UNDEFINED:
			return "undefined{}";
		case T_NULL:
			return "null{}";
		case T_BOOL:
			return "bool{" + (std::string)( ( (value::Bool*)this )->value
				? "true"
				: "false"
			) + "}";
		case T_INT:
			return "int{" + std::to_string( ( (value::Int*)this )->value ) + "}";
		case T_FLOAT:
			return "float{" + std::to_string( ( (value::Float*)this )->value ) + "}";
		case T_STRING:
			return "string{" + ( (value::String*)this )->value + "}";
		case T_ARRAY: {
			std::string str = "";
			str.append( "array{" );
			bool first = true;
			for ( const auto& it : ( (value::Array*)this )->value ) {
				if ( first ) {
					first = false;
				}
				else {
					str.append( "," );
				}
				str.append( it->Dump() );
			}
			str.append( "}" );
			return str;
		}
		case T_OBJECT: {
			const auto* obj = (value::Object*)this;
			std::string str = "";
			str.append( "object(" + obj->object_class + "){" );
			bool first = true;
			for ( const auto& it : obj->value ) {
				if ( first ) {
					first = false;
				}
				else {
					str.append( "," );
				}
				str.append( it.first + ":" + it.second->Dump() );
			}
			str.append( "}" );
			return str;
		}
		case T_CALLABLE:
			return "callable{}"; // TODO
		case T_ARRAYREF: {
			const auto* that = (value::ArrayRef*)this;
			return "arrayref{" + std::to_string( (ptr_int_t)that->array ) + "," + std::to_string( that->index ) + +"}";
		}
		case T_ARRAYRANGEREF: {
			const auto* that = (value::ArrayRangeRef*)this;
			return "arrayrangeref{" + std::to_string( (ptr_int_t)that->array ) + "," + (
				that->from.has_value()
					? std::to_string( that->from.value() )
					: ""
			) + (
				that->to.has_value()
					? std::to_string( that->to.value() )
					: ""
			) + +"}";
		}
		case T_OBJECTREF: {
			const auto* that = (value::ObjectRef*)this;
			return "objectref{" + std::to_string( (ptr_int_t)that->object ) + "," + that->key + "}";
		}
		case T_VALUEREF: {
			const auto* that = (value::ValueRef*)this;
			return "valueref{" + std::to_string( (ptr_int_t)that->target ) + "}";
		}
		case T_RANGE: {
			const auto* that = (value::Range*)this;
			return "range{" + (
				that->from
					? std::to_string( *that->from )
					: ""
			) + ":" + (
				that->to
					? std::to_string( *that->to )
					: ""
			) + "}";
		}
		case T_LOOPCONTROL: {
			return "loopcontrol{" + ToString() + "}";
		}
		default:
			return "unknown{" + std::to_string( type ) + "}";
	}
}

Value* const Value::Deref() {
	switch ( type ) {
		case T_ARRAYREF: {
			const auto* that = (value::ArrayRef*)this;
			return that->array->Get( that->index );
		}
		case T_ARRAYRANGEREF: {
			THROW( "deref of array range refs not supported here" );
		}
		case T_OBJECTREF: {
			const auto* that = (value::ObjectRef*)this;
			return that->object->Get( that->key );
		}
		case T_VALUEREF: {
			const auto* that = (value::ValueRef*)this;
			return that->target;
		}
		default:
			return this;
	}
}

Value* const Value::Clone() {
	return New( this );
}

#define DEFAULT_COMPARE( _op ) \
          case T_BOOL: \
                return ( (value::Bool*)this )->value _op ( (value::Bool*)&other )->value; \
          case T_INT: \
                return ( (value::Int*)this )->value _op ( (value::Int*)&other )->value; \
          case T_FLOAT: \
                return ( (value::Float*)this )->value _op ( (value::Float*)&other )->value; \
          case T_STRING: \
                return ( (value::String*)this )->value _op ( (value::String*)&other )->value; \
          default: \
                THROW( "operator " #_op " not implemented for type " + GetTypeString() );
#define DEFAULT_COMPARE_NE( _op ) \
     if ( type != other.type ) { \
          THROW( "can't compare type " + GetTypeStringStatic( type ) + " to type " + GetTypeStringStatic( other.type ) + " using operator " #_op ); \
     } \
     switch ( type ) { \
          case T_UNDEFINED: \
                return false; \
          case T_NULL: \
                return false; \
          DEFAULT_COMPARE( _op ) \
     }

const bool Value::operator==( const Value& other ) const {
	if ( type != other.type ) {
		return false;
	}
	switch ( type ) {
		case T_UNDEFINED:
			return true;
		case T_NULL:
			return true;
		case T_ARRAY: {
			const auto& a = ( (const value::Array*)this )->value;
			const auto& b = ( (const value::Array*)&other )->value;
			if ( a.size() != b.size() ) {
				return false;
			}
			else {
				for ( size_t i = 0 ; i < a.size() ; i++ ) {
					if ( ( *a[ i ]->Deref() != *b[ i ]->Deref() ) ) {
						return false;
					}
				}
				return true;
			}
		}
		case T_OBJECT: {
			const auto* obj_a = (const value::Object*)this;
			const auto* obj_b = (const value::Object*)&other;
			if ( obj_a->object_class != obj_b->object_class ) {
				return false;
			}
			if ( obj_a->wrapobj || obj_b->wrapobj ) {
				return obj_a->wrapobj == obj_b->wrapobj;
			}
			const auto& a = obj_a->value;
			const auto& b = obj_b->value;
			if ( a.size() != b.size() ) {
				return false;
			}
			value::object_properties_t::const_iterator it_b;
			for ( const auto& it : a ) {
				if (
					( it_b = b.find( it.first ) ) == b.end() ||
						( *it_b->second->Deref() != *it.second->Deref() )
					) {
					return false;
				}
			}
			return true;
		}
		case T_CALLABLE: {
			const auto* obj_a = (const value::Callable*)this;
			const auto* obj_b = (const value::Callable*)&other;
			return obj_a == obj_b;
		}
		DEFAULT_COMPARE( == )
	}
}

const bool Value::operator!=( const Value& other ) const {
	return !operator==( other );
}

const bool Value::operator<( const Value& other ) const {
	DEFAULT_COMPARE_NE( < )
}

const bool Value::operator>( const Value& other ) const {
	DEFAULT_COMPARE_NE( > )
}

const bool Value::operator<=( const Value& other ) const {
	DEFAULT_COMPARE_NE( <= )
}

const bool Value::operator>=( const Value& other ) const {
	DEFAULT_COMPARE_NE( >= )
}

#undef DEFAULT_COMPARE
#undef DEFAULT_COMPARE_NE

const std::string Value::GCString() const {
	return gc::Object::GCString() + "( " + ToString() + " )";
}

Value* const Value::New( const Value* value ) {
	auto* gc_space = value->m_gc_space;
	ASSERT_NOLOG( gc_space, "value gc space is null" );
	switch ( type ) {
		case T_UNDEFINED:
			return VALUE( value::Undefined );
		case T_NULL:
			return VALUE( value::Null );
		case T_BOOL:
			return VALUE( value::Bool, , ( (value::Bool*)value )->value );
		case T_INT:
			return VALUE( value::Int, , ( (value::Int*)value )->value );
		case T_FLOAT:
			return VALUE( value::Float, , ( (value::Float*)value )->value );
		case T_STRING:
			return VALUE( value::String, , ( (value::String*)value )->value );
		case T_ARRAY: {
			value::array_elements_t elements = {};
			const auto* arr = (value::Array*)value;
			for ( const auto& it : arr->value ) {
				elements.push_back( it->Clone() );
			}
			return VALUE( value::Array, , elements );
		}
		case T_OBJECT: {
			value::object_properties_t properties = {};
			const auto* obj = (value::Object*)value;
			for ( const auto& it : obj->value ) {
				properties.insert_or_assign( it.first, it.second->Clone() );
			}
			return VALUE( value::Object, , nullptr, properties, obj->object_class, obj->wrapobj, obj->wrapsetter );
		}
		case T_CALLABLE:
			return this; // ?????
		case T_ARRAYREF:
		case T_ARRAYRANGEREF:
		case T_OBJECTREF:
		case T_VALUEREF: {
			// no need to keep ref to old value if it's a copy
			return Deref();
		}
		case T_RANGE:
			THROW( "ranges are not supposed to be cloned" );
		case T_LOOPCONTROL:
			THROW( "loop controls are not supposed to be cloned" );
		default: {
			THROW( "unsupported value type: " + ToString() );
		}
	}
}

void Value::Serialize( types::Buffer* buf, Value* const value ) {
	buf->WriteInt( value->type );
	switch ( value->type ) {
		case T_UNDEFINED:
			break;
		case T_NULL:
			break;
		case T_BOOL: {
			buf->WriteBool( ( (value::Bool*)value )->value );
			break;
		}
		case T_INT: {
			buf->WriteInt( ( (value::Int*)value )->value );
			break;
		}
		case T_FLOAT: {
			buf->WriteFloat( ( (value::Float*)value )->value );
			break;
		}
		case T_STRING: {
			buf->WriteString( ( (value::String*)value )->value );
			break;
		}
		case T_ARRAY: {
			const auto& elements = ( (value::Array*)value )->value;
			buf->WriteInt( elements.size() );
			for ( const auto& e : elements ) {
				Value::Serialize( buf, e );
			}
			break;
		}
		case T_OBJECT: {
			const auto* obj = (value::Object*)value;
			ASSERT_NOLOG( obj->object_class.empty(), "serialization of custom object classes is not supported" );
			ASSERT_NOLOG( !obj->wrapobj, "serialization of objects with wrapobj is not supported" );
			ASSERT_NOLOG( !obj->wrapsetter, "serialization of objects with wrapsetter is not supported" );
			const auto& properties = obj->value;
			buf->WriteInt( properties.size() );
			for ( const auto& p : properties ) {
				buf->WriteString( p.first );
				Value::Serialize( buf, p.second );
			}
			break;
		}
		default:
			THROW( "invalid/unsupported type for serialization: " + value->GetTypeString() );
	}
}

Value* Value::Unserialize( gc::Space* const gc_space, types::Buffer* buf ) {
	type_t type = (type_t)buf->ReadInt();
	switch ( type ) {
		case T_UNDEFINED:
			return VALUE( value::Undefined );
		case T_NULL:
			return VALUE( value::Null );
		case T_BOOL:
			return VALUE( value::Bool, , buf->ReadBool() );
		case T_INT:
			return VALUE( value::Int, , buf->ReadInt() );
		case T_FLOAT:
			return VALUE( value::Float, , buf->ReadFloat() );
		case T_STRING:
			return VALUE( value::String, , buf->ReadString() );
		case T_ARRAY: {
			value::array_elements_t elements = {};
			const auto size = buf->ReadInt();
			elements.reserve( size );
			for ( size_t i = 0 ; i < size ; i++ ) {
				elements.push_back( Value::Unserialize( gc_space, buf ) );
			}
			return VALUE( value::Array, , elements );
		}
		case T_OBJECT: {
			value::object_properties_t properties = {};
			const auto size = buf->ReadInt();
			for ( size_t i = 0 ; i < size ; i++ ) {
				const auto k = buf->ReadString();
				properties.insert(
					{
						k,
						Value::Unserialize( gc_space, buf )
					}
				);
			}
			return VALUE( value::Object, , nullptr, properties );
		}
		default:
			THROW( "invalid/unsupported type for unserialization: " + GetTypeStringStatic( type ) );
	}
}

Value::Value( gc::Space* const gc_space, const type_t type )
	: gc::Object( gc_space )
	, type( type )
	, m_gc_space( gc_space ) {}

}
