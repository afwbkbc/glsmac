#pragma once

#include "gse/type/Type.h"

namespace gse {
class GSE;
namespace runnable {

class Runnable : public type::Type {
public:
	Runnable()
		: type::Type( type::Type::T_CALLABLE ) {}

	virtual ~Runnable() {}

	virtual type::Type& Run( GSE* gse ) = 0;

};

}
}

#include "gse/GSE.h"