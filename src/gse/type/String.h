#pragma once

#include <string>

#include "Type.h"

namespace gse {
namespace type {

class String : public Type {
public:

	static const type_t GetType() { return Type::T_STRING; }

	String( const std::string& initial_value )
		: Type( GetType() )
		, value( initial_value ) {
	}

	std::string value;
};

}
}
