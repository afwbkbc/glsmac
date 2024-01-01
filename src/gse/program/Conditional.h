#pragma once

#include "Element.h"

namespace gse {
namespace program {

class Conditional : public Control {
public:
	enum conditional_type_t {
		CT_IF,
		CT_ELSEIF,
		CT_ELSE,
		CT_WHILE,
		CT_TRY,
		CT_CATCH,
	};

	Conditional( const si_t& si, const conditional_type_t conditional_type )
		: Control( si, CT_CONDITIONAL )
		, conditional_type( conditional_type ) {}

	const conditional_type_t conditional_type;

	virtual ~Conditional() = default;
};

}
}
