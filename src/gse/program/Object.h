#pragma once

#include <string>
#include <unordered_map>

#include "Operand.h"

namespace gse {
namespace program {

class Expression;

class Object : public Operand {
public:
	typedef std::unordered_map< std::string, const Expression* > properties_t;

	Object( const si_t& si, const properties_t& properties );
	~Object();

	const properties_t properties;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

