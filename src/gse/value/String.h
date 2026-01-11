#pragma once

#include <string>

#include "gse/Value.h"

namespace gse {
namespace value {

class String : public Value {
public:

	static const value_type_t GetType() { return VT_STRING; }

	String( gc::Space* const gc_space, const std::string& initial_value )
		: Value( gc_space, GetType() )
		, value( initial_value ) {
	}

	std::string value;

};

}
}
