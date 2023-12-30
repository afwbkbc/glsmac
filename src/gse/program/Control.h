#pragma once

#include "Element.h"

#include "Expression.h"

namespace gse {
namespace program {

class Control : public Element {
public:
	enum control_type_t {
		CT_STATEMENT,
		CT_CONDITIONAL,
	};
	Control( const control_type_t control_type )
		: Element( ET_CONDITIONAL )
		, control_type( control_type ) {}

	const control_type_t control_type;

	virtual ~Control() = default;

};

}
}

