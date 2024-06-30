#pragma once

#include "Operand.h"

namespace gse {
namespace program {

class Expression;

class Condition : public Element {
public:
	enum condition_type_t {
		CT_NOTHING,
		CT_SIMPLE,
	};

	Condition( const si_t& si, const condition_type_t type );
	virtual ~Condition() = default;

	const condition_type_t type;

};

}
}
