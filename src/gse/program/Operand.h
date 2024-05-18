#pragma once

#include "Element.h"

namespace gse {
namespace program {

class Operand : public Element {
public:
	enum operand_type_t {
		OT_NOTHING,
		OT_VALUE,
		OT_VARIABLE,
		OT_ARRAY,
		OT_OBJECT,
		OT_SCOPE,
		OT_EXPRESSION,
		OT_FUNCTION,
		OT_CALL,
	};

	Operand( const si_t& si, const operand_type_t type );

	const operand_type_t type;

	virtual ~Operand() = default;
};

}
}
