#pragma once

#include "base/Base.h"

namespace gse {
namespace program {

class Element {
public:

	enum element_type_t {
		ET_NOTHING,
		ET_VALUE,
		ET_VARIABLE,
		ET_EXPRESSION,
		ET_OPERAND,
		ET_OPERATOR,
		ET_OBJECT,
		ET_FUNCTION,
		ET_CALL,
		ET_SCOPE,
		ET_STATEMENT,
	};
	Element( const element_type_t element_type )
		: m_element_type( element_type ) {}

	const element_type_t m_element_type;

	virtual ~Element() = default;

	virtual const std::string ToString( const size_t depth = 0 ) const = 0;

protected:
	const std::string Formatted( const std::string& text, const size_t depth ) const {
		return std::string( TOSTRING_PREFIX_SIZE * depth, ' ' ) + text + "\n";
	}

private:
	const size_t TOSTRING_PREFIX_SIZE = 4;

};

}
}
