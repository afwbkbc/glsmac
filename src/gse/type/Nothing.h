#pragma once

#include "Type.h"

namespace gse {
namespace type {

class Nothing : public Type {
public:

	static const type_t GetType() { return Type::T_NOTHING; }

	Nothing()
		: Type( GetType() ) {}

	Nothing( const Nothing& other )
		: Nothing() {}
};

}
}
