#pragma once

#include <unordered_map>

#include "Element.h"

#include "Types.h"

namespace gse {
namespace program {

class Operator : public Element {
public:
	Operator( const si_t& si, const operator_type_t op );

	const operator_type_t op;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;

};

}
}
