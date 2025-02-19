#pragma once

#include <string>

#include "gse/Value.h"

namespace gse {
namespace value {

class String : public Value {
public:

	static const type_t GetType() { return Value::T_STRING; }

	String( const std::string& initial_value )
		: Value( GetType() )
		, value( initial_value ) {
	}

	std::string value;
};

}
}
