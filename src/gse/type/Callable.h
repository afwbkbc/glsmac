#pragma once

#include "gse/type/Type.h"
#include "gse/Value.h"

namespace gse {
class GSE;
namespace runnable {

class Callable : public type::Type {
public:
	Callable()
		: type::Type( type::Type::T_CALLABLE ) {}

	virtual ~Callable() {}

	virtual Value Run( GSE* gse ) = 0;

};

}
}

#include "gse/GSE.h"