#pragma once

#include "Type.h"

namespace gse {
namespace type {

class Bool : public Type {
public:

	static const type_t GetType() { return Type::T_BOOL; }

	Bool( const bool initial_value )
		: Type( GetType() )
		, value( initial_value ) {
	}

	bool value;
};

}
}
