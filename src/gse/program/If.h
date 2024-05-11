#pragma once

#include "Conditional.h"

namespace gse {
namespace program {

class Expression;
class Scope;

class If : public Conditional {
public:

	If( const si_t& si, const Expression* condition, const Scope* body, const Conditional* els = nullptr );
	~If();

	const Expression* condition;
	const Scope* body;
	const Conditional* els;

	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

