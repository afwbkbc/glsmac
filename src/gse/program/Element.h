#pragma once

#include "gse/Types.h"

namespace gse {
namespace program {

class Element {
public:
	enum element_type_t {
		ET_OPERAND,
		ET_OPERATOR,
		ET_CONDITIONAL,
	};
	Element( const si_t& si, const element_type_t element_type );
	virtual ~Element() = default;

	const element_type_t m_element_type;
	const si_t m_si;

	virtual const std::string ToString() const;
	virtual const std::string Dump( const size_t depth = 0 ) const = 0;

protected:

	const std::string Formatted( const std::string& text, const size_t depth ) const {
		return std::string( TOSTRING_PREFIX_SIZE * depth, ' ' ) + text + "\n";
	}

private:
	const size_t TOSTRING_PREFIX_SIZE = 4;

};

}
}
