#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Operand.h"

namespace gse {
namespace program {

class Expression;

class Object : public Operand {
public:
	typedef std::unordered_map< std::string, const Expression* > properties_t;
	typedef std::vector< std::pair< std::string, const Expression* > > ordered_properties_t;

	Object( const si_t& si, const ordered_properties_t& ordered_properties );
	~Object();

	properties_t properties = {};
	const ordered_properties_t ordered_properties;

	const std::string ToString() const override;
	const std::string Dump( const size_t depth = 0 ) const override;
};

}
}

