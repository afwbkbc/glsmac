#pragma once

#include "Control.h"

namespace gse {
namespace program {

class Expression;

class Statement : public Control {
public:
	Statement( const si_t& si, const Expression* body );
	~Statement();

	const Expression* body;

	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}
