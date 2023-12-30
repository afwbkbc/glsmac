#pragma once

#include "Element.h"

namespace gse {
namespace program {

class Conditional : public Control {
public:
	enum conditional_type_t {
		CT_IF,
		CT_ELSEIF,
		CT_ELSE
	};

	Conditional( const conditional_type_t conditional_type )
		: Control( CT_CONDITIONAL )
		, conditional_type( conditional_type ) {}

	const conditional_type_t conditional_type;

	virtual ~Conditional() = default;
};

}
}
