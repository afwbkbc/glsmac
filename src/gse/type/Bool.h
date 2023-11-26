#pragma once

#include "Type.h"

namespace gse {
namespace type {

class Bool : public Type {
public:
	Bool( const bool initial_value )
		: Type( Type::T_BOOL )
		, value( initial_value ) {
	}

	bool value;
};

}
}
