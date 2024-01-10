#include "Type.h"

#include "base/Base.h"

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

namespace gse {
namespace type {

const std::string Type::GetTypeString( const type_t type ) {
	switch ( type ) {
		case T_UNDEFINED:
			return "undefined";
		case T_NULL:
			return "null";
		case T_BOOL:
			return "bool";
		case T_INT:
			return "int";
		case T_FLOAT:
			return "float";
		case T_STRING:
			return "string";
		case T_ARRAY:
			return "array";
		case T_OBJECT:
			return "object";
		case T_CALLABLE:
			return "callable";
		case T_ARRAYREF:
			return "arrayref";
		case T_ARRAYRANGEREF:
			return "arrayrangeref";
		case T_OBJECTREF:
			return "objectref";
		case T_RANGE:
			return "range";
		default:
			return "unknown";
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
			std::string str = "";
			str.append( "{ " );
			bool first = true;
			for ( const auto& it : ( (Object*)this )->value ) {
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
			return "callable"; // TODO
		case T_ARRAYREF:
		case T_ARRAYRANGEREF:
		case T_OBJECTREF:
			THROW( "refs are not intended to be printed" );
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
			THROW( "unknown is not intented to be printed" );
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
			std::string str = "";
			str.append( "object{" );
			bool first = true;
			for ( const auto& it : ( (Object*)this )->value ) {
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
			return "arrayref{" + std::to_string( (long)that->array ) + "," + std::to_string( that->index ) + +"}";
		}
		case T_ARRAYRANGEREF: {
			const auto* that = (ArrayRangeRef*)this;
			return "arrayrangeref{" + std::to_string( (long)that->array ) + "," + (
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
			return "objectref{" + std::to_string( (long)that->object ) + "," + that->key + "}";
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
            THROW( "operator " #_op " not implemented for type " + std::to_string( type ) );
#define DEFAULT_COMPARE_NE( _op ) \
    if ( type != other.type ) { \
        THROW( "can't compare type " + std::to_string( type ) + " to type " + std::to_string( other.type ) + " using operator " #_op ); \
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

}
}
