#pragma once

#include "Control.h"

#include "Expression.h"

namespace gse {
namespace program {

class Statement : public Control {
public:
	Statement( const si_t& si, const Expression* body )
		: Control( si, CT_STATEMENT )
		, body( body ) {}

	const Expression* body;

	~Statement() {
		delete body;
	}

	const std::string Dump( const size_t depth = 0 ) const override {
		return Formatted( "Statement" + m_si.ToString() + "(", depth ) +
			body->Dump( depth + 1 ) +
			Formatted( ")", depth );
	}
};

}
}

