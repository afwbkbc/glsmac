#pragma once

#include "Element.h"

namespace gse {
namespace program {

class Operator : public Element {
public:
	enum operator_type_t {
		OT_NOOP,
		OT_RETURN,
		OT_EQUALS,
		OT_ASSIGN,
		OT_ADD,
		OT_SUB,
		OT_INC,
		OT_DEC,
		OT_INCBY,
		OT_DECBY,
		OT_CHILD,
	};

	Operator( const operator_type_t op )
		: op( op ) {}

	const operator_type_t op;
};

}
}

