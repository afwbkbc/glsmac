#include "Type.h"

#include "base/Base.h"

#include "Null.h"
#include "Bool.h"
#include "Int.h"
#include "String.h"
#include "Object.h"

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
		case T_STRING:
			return "string";
		case T_OBJECT:
			return "object";
		case T_CALLABLE:
			return "callable";
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
			return "bool{" + std::to_string( ( (Bool*)this )->value ) + "}";
		case T_INT:
			return "int{" + std::to_string( ( (Int*)this )->value ) + "}";
		case T_STRING:
			return "string{" + ( (String*)this )->value + "}";
		case T_OBJECT: {
			std::string str = "";
			// TODO: str.reserve() ?
			str.append( "object{" );
			bool first = true;
			for ( const auto& it : ( (Object*)this )->value ) {
				if ( first ) {
					first = false;
				}
				else {
					str.append( "," );
				}
				str.append( it.first + "=" + it.second.ToString() );
			}
			str.append( "}" );
			return str;
		}
		case T_CALLABLE:
			return "callable"; // TODO
		default:
			return "unknown";
	}
}

const bool Type::operator==( const Type& other ) const {
	return ToString() == other.ToString(); // TODO: better checks
}

}
}
