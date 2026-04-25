#pragma once

#include <vector>

#include "Conditional.h"

namespace gse {
namespace program {

class SimpleCondition;
class Case;

class Switch : public Conditional {
public:

	Switch( const si_t& si, const SimpleCondition* condition, const std::vector< Case* >& cases );
	~Switch();

	const SimpleCondition* condition;
	const std::vector< Case* > cases;

	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

