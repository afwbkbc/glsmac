#pragma once

#include "Control.h"

namespace gse {
namespace program {

class Conditional : public Control {
public:
	enum conditional_type_t {
		CT_IF,
		CT_ELSEIF,
		CT_ELSE,
		CT_WHILE,
		CT_FOR,
		CT_TRY,
		CT_CATCH,
		CT_SWITCH,
		CT_CASE,
	};

	Conditional( const si_t& si, const conditional_type_t conditional_type );
	virtual ~Conditional() = default;

	const conditional_type_t conditional_type;

};

}
}
