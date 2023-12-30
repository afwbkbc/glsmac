#pragma once

#include "base/Base.h"

namespace gse {
namespace program {

class Element {
public:
	enum element_type_t {
		ET_OPERAND,
		ET_OPERATOR,
		ET_CONDITIONAL,
	};
	Element( const element_type_t element_type )
		: m_element_type( element_type ) {}

	const element_type_t m_element_type;

	virtual ~Element() = default;

	virtual const std::string ToString( const size_t depth = 0 ) const = 0;

protected:
	const std::string Formatted( const std::string& text, const size_t depth ) const {
		return std::string( TOSTRING_PREFIX_SIZE * depth, ' ' ) + text + ( depth > 0
			? "\n"
			: ""
		);
	}

private:
	const size_t TOSTRING_PREFIX_SIZE = 4;

};

}
}
