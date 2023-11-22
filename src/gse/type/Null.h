#pragma once

#include "Type.h"

namespace gse {
namespace type {

class Null : public Type {
public:
	Null()
		: Type( Type::T_NULL ) {}

	Null( const Null& other )
		: Null() {}
};

}
}
