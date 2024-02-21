#pragma once

#include "base/Base.h"

namespace gse {

// source info, this is how source elements (and later programs) are linked to source code
struct si_t {
	std::string file;
	struct pos_t {
		size_t line;
		size_t col;
	};
	pos_t from;
	pos_t to;
	const std::string ToString() const {
		return "[" + file + ":" + std::to_string( from.line ) + ":" + std::to_string( from.col ) + "]";
	}
};

namespace program {

class Element {
public:
	enum element_type_t {
		ET_OPERAND,
		ET_OPERATOR,
		ET_CONDITIONAL,
	};
	Element( const si_t& si, const element_type_t element_type )
		: m_si( si )
		, m_element_type( element_type ) {}

	const element_type_t m_element_type;
	const si_t m_si;

	virtual ~Element() = default;

	virtual const std::string ToString() const {
		THROW( "ToString() not implemented for element: " + Dump() );
	}
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
