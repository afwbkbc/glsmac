#pragma once

#include "Conditional.h"

#include "Scope.h"

namespace gse {
namespace program {

class Scope;
class Catch;

class Try : public Conditional {
public:

	Try( const si_t& si, const Scope* body, const Catch* handlers );
	~Try();

	const Scope* body;
	const Catch* handlers;

	const std::string Dump( const size_t depth = 0 ) const override {
		return Formatted( "Try" + m_si.ToString() + "(", depth ) +
			Formatted( "Body(", depth + 1 ) +
			body->Dump( depth + 2 ) +
			Formatted( ")", depth + 1 ) +
			Formatted( ")", depth );
	}
};

}
}

