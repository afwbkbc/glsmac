#include "Type.h"

#include "common/Common.h"

#include "Undefined.h"
#include "Null.h"
#include "Bool.h"
#include "Int.h"
#include "Float.h"
#include "String.h"
#include "Array.h"
#include "Object.h"
#include "ArrayRef.h"
#include "ArrayRangeRef.h"
#include "ObjectRef.h"
#include "Range.h"
#include "Exception.h"

#include "types/Buffer.h"

namespace gse {
namespace type {

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
static const std::string s_t_range = "Range";
static const std::string s_t_unknown = "Unknown";
const std::string& Type::GetTypeString( const type_t type ) {
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
		case T_RANGE:
			return s_t_range;
		default:
			return s_t_unknown;
	}
}

const std::string Type::ToString() const {
	switch ( type ) {
		case T_UNDEFINED:
			return "undefined";
		case T_NULL:
			return "null";
		case T_BOOL:
			return ( (Bool*)this )->value
				? "true"
				: "false";
		case T_INT:
			return std::to_string( ( (Int*)this )->value );
		case T_FLOAT:
			return std::to_string( ( (Float*)this )->value );
		case T_STRING:
			return ( (String*)this )->value;
		case T_ARRAY: {
			std::string str = "";
			str.append( "[ " );
			bool first = true;
			for ( const auto& it : ( (Array*)this )->value ) {
				if ( first ) {
					first = false;
				}
				else {
					str.append( ", " );
				}
				str.append( it.ToString() );
			}
			str.append( " ]" );
			return str;
		}
		case T_OBJECT: {
			const auto* obj = (Object*)this;
			std::string str = "";
			str.append( Object::GetClassString( obj->object_class ) + "{ " );
			bool first = true;
			for ( const auto& it : obj->value ) {
				if ( first ) {
					first = false;
				}
				else {
					str.append( ", " );
				}
				str.append( it.first + ": " + it.second.ToString() );
			}
			str.append( " }" );
			return str;
		}
		case T_CALLABLE:
			return "()";
		case T_ARRAYREF: {
			const auto* that = (ArrayRef*)this;
			return that->array->Get( that->index ).ToString();
		}
		case T_ARRAYRANGEREF: {
			const auto* that = (ArrayRangeRef*)this;
			return that->array->GetSubArray( that->from, that->to ).ToString();
		}
		case T_OBJECTREF: {
			const auto* that = (ObjectRef*)this;
			return that->object->Get( that->key ).ToString();
		}
		case T_RANGE: {
			const auto* that = (Range*)this;
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
		default:
			THROW( "unknown is not intended to be printed" );
	}
}

const std::string Type::Dump() const {
	switch ( type ) {
		case T_UNDEFINED:
			return "undefined{}";
		case T_NULL:
			return "null{}";
		case T_BOOL:
			return "bool{" + (std::string)( ( (Bool*)this )->value
				? "true"
				: "false"
			) + "}";
		case T_INT:
			return "int{" + std::to_string( ( (Int*)this )->value ) + "}";
		case T_FLOAT:
			return "float{" + std::to_string( ( (Float*)this )->value ) + "}";
		case T_STRING:
			return "string{" + ( (String*)this )->value + "}";
		case T_ARRAY: {
			std::string str = "";
			str.append( "array{" );
			bool first = true;
			for ( const auto& it : ( (Array*)this )->value ) {
				if ( first ) {
					first = false;
				}
				else {
					str.append( "," );
				}
				str.append( it.Get()->Dump() );
			}
			str.append( "}" );
			return str;
		}
		case T_OBJECT: {
			const auto* obj = (Object*)this;
			std::string str = "";
			str.append( "object" + Object::GetClassString( obj->object_class ) + "{" );
			bool first = true;
			for ( const auto& it : obj->value ) {
				if ( first ) {
					first = false;
				}
				else {
					str.append( "," );
				}
				str.append( it.first + ":" + it.second.Get()->Dump() );
			}
			str.append( "}" );
			return str;
		}
		case T_CALLABLE:
			return "callable{}"; // TODO
		case T_ARRAYREF: {
			const auto* that = (ArrayRef*)this;
			return "arrayref{" + std::to_string( (ptr_int_t)that->array ) + "," + std::to_string( that->index ) + +"}";
		}
		case T_ARRAYRANGEREF: {
			const auto* that = (ArrayRangeRef*)this;
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
			const auto* that = (ObjectRef*)this;
			return "objectref{" + std::to_string( (ptr_int_t)that->object ) + "," + that->key + "}";
		}
		case T_RANGE: {
			const auto* that = (Range*)this;
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
		default:
			return "unknown{" + std::to_string( type ) + "}";
	}
}

const Type* Type::Deref() const {
	switch ( type ) {
		case T_ARRAYREF: {
			const auto* that = (ArrayRef*)this;
			return that->array->Get( that->index ).Get();
		}
		case T_ARRAYRANGEREF: {
			THROW( "deref of array range refs not supported here" );
		}
		case T_OBJECTREF: {
			const auto* that = (ObjectRef*)this;
			return that->object->Get( that->key ).Get();
		}
		default:
			return this;
	}
}

#define DEFAULT_COMPARE( _op ) \
        case T_BOOL: \
            return ( (Bool*)this )->value _op ( (Bool*)&other )->value; \
        case T_INT: \
            return ( (Int*)this )->value _op ( (Int*)&other )->value; \
        case T_FLOAT: \
            return ( (Float*)this )->value _op ( (Float*)&other )->value; \
        case T_STRING: \
            return ( (String*)this )->value _op ( (String*)&other )->value; \
        default: \
            THROW( "operator " #_op " not implemented for type " + GetTypeString( type ) );
#define DEFAULT_COMPARE_NE( _op ) \
    if ( type != other.type ) { \
        THROW( "can't compare type " + GetTypeString( type ) + " to type " + GetTypeString( other.type ) + " using operator " #_op ); \
    } \
    switch ( type ) { \
        case T_UNDEFINED: \
            return false; \
        case T_NULL: \
            return false; \
        DEFAULT_COMPARE( _op ) \
    }

const bool Type::operator==( const Type& other ) const {
	if ( type != other.type ) {
		return false;
	}
	switch ( type ) {
		case T_UNDEFINED:
			return true;
		case T_NULL:
			return true;
		case T_ARRAY: {
			const auto& a = ( (const Array*)this )->value;
			const auto& b = ( (const Array*)&other )->value;
			if ( a.size() != b.size() ) {
				return false;
			}
			else {
				for ( size_t i = 0 ; i < a.size() ; i++ ) {
					if ( ( *a[ i ].Get()->Deref() != *b[ i ].Get()->Deref() ) ) {
						return false;
					}
				}
				return true;
			}
		}
		case T_OBJECT: {
			const auto* obj_a = (const Object*)this;
			const auto* obj_b = (const Object*)&other;
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
			object_properties_t::const_iterator it_b;
			for ( const auto& it : a ) {
				if (
					( it_b = b.find( it.first ) ) == b.end() ||
						( *it_b->second.Get()->Deref() != *it.second.Get()->Deref() )
					) {
					return false;
				}
			}
			return true;
		}
		DEFAULT_COMPARE( == )
	}
}

const bool Type::operator!=( const Type& other ) const {
	return !operator==( other );
}

const bool Type::operator<( const Type& other ) const {
	DEFAULT_COMPARE_NE( < )
}

const bool Type::operator>( const Type& other ) const {
	DEFAULT_COMPARE_NE( > )
}

const bool Type::operator<=( const Type& other ) const {
	DEFAULT_COMPARE_NE( <= )
}

const bool Type::operator>=( const Type& other ) const {
	DEFAULT_COMPARE_NE( >= )
}

#undef DEFAULT_COMPARE
#undef DEFAULT_COMPARE_NE

void Type::Serialize( types::Buffer* buf, const Type* type ) {
	buf->WriteInt( type->type );
	switch ( type->type ) {
		case T_UNDEFINED:
			break;
		case T_NULL:
			break;
		case T_BOOL: {
			buf->WriteBool( ( (Bool*)type )->value );
			break;
		}
		case T_INT: {
			buf->WriteInt( ( (Int*)type )->value );
			break;
		}
		case T_FLOAT: {
			buf->WriteFloat( ( (Float*)type )->value );
			break;
		}
		case T_STRING: {
			buf->WriteString( ( (String*)type )->value );
			break;
		}
		case T_ARRAY: {
			const auto& elements = ( (Array*)type )->value;
			buf->WriteInt( elements.size() );
			for ( const auto& e : elements ) {
				Value::Serialize( buf, e );
			}
			break;
		}
		case T_OBJECT: {
			const auto* obj = (Object*)type;
			ASSERT_NOLOG( obj->object_class == Object::CLASS_NONE, "serialization of custom object classes is not supported" );
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
			THROW( "invalid/unsupported type for serialization: " + GetTypeString( type->type ) );
	}
}

Value Type::Unserialize( types::Buffer* buf ) {
	type_t type = (type_t)buf->ReadInt();
	switch ( type ) {
		case T_UNDEFINED:
			return VALUE( Undefined );
		case T_NULL:
			return VALUE( Null );
		case T_BOOL:
			return VALUE( Bool, buf->ReadBool() );
		case T_INT:
			return VALUE( Int, buf->ReadInt() );
		case T_FLOAT:
			return VALUE( Float, buf->ReadFloat() );
		case T_STRING:
			return VALUE( String, buf->ReadString() );
		case T_ARRAY: {
			array_elements_t elements = {};
			const auto size = buf->ReadInt();
			elements.reserve( size );
			for ( size_t i = 0 ; i < size ; i++ ) {
				elements.push_back( Value::Unserialize( buf ) );
			}
			return VALUE( Array, elements );
		}
		case T_OBJECT: {
			object_properties_t properties = {};
			const auto size = buf->ReadInt();
			for ( size_t i = 0 ; i < size ; i++ ) {
				const auto k = buf->ReadString();
				properties.insert(
					{
						k,
						Value::Unserialize( buf )
					}
				);
			}
			return VALUE( Object, properties );
		}
		default:
			THROW( "invalid/unsupported type for unserialization: " + GetTypeString( type ) );
	}
}

}
}
