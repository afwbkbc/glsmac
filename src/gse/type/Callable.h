#pragma once

#include <vector>

#include "gse/type/Type.h"
#include "gse/Value.h"

namespace gse {
class GSE;
namespace type {

class Callable : public type::Type {
public:
	typedef const std::vector< Value > function_arguments_t;

	Callable()
		: type::Type( type::Type::T_CALLABLE ) {}

	virtual ~Callable() {}

	virtual Value Run( GSE* gse, const Callable::function_arguments_t arguments ) = 0;

};

}
}

#include "gse/GSE.h"