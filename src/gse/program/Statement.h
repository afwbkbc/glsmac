#pragma once

#include "Element.h"

#include "Expression.h"

namespace gse {
namespace program {

class Statement : public Element {
public:
	Statement( const Expression* body )
		: body( body ) {}

	const Expression* body;

	~Statement() {
		delete body;
	}
};

}
}

