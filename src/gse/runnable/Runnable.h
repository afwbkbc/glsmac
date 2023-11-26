#pragma once

#include "gse/type/Type.h"
#include "gse/Value.h"

namespace gse {
class GSE;
namespace runnable {

class Runnable : public type::Type {
public:
	Runnable()
		: type::Type( type::Type::T_CALLABLE ) {}

	virtual ~Runnable() {}

	virtual Value Run( GSE* gse ) = 0;

};

}
}

#include "gse/GSE.h"