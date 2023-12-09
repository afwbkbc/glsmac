#pragma once

#include "Type.h"

namespace gse {
namespace type {

class Undefined : public Type {
public:

	static const type_t GetType() { return Type::T_UNDEFINED; }

	Undefined()
		: Type( GetType() ) {}

	Undefined( const Undefined& other )
		: Undefined() {}
};

}
}
