#pragma once

#include "Element.h"

namespace gse {
namespace program {

class Control : public Element {
public:
	enum control_type_t {
		CT_STATEMENT,
		CT_CONDITIONAL,
	};
	
	Control( const si_t& si, const control_type_t control_type );

	const control_type_t control_type;

	virtual ~Control() = default;

};

}
}

