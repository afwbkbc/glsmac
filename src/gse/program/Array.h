#pragma once

#include <vector>

#include "Operand.h"
#include "Expression.h"

namespace gse {
namespace program {

class Array : public Operand {
public:
	typedef std::vector< const Expression* > elements_t;

	Array( const si_t& si, const elements_t& elements )
		: Operand( si, OT_ARRAY )
		, elements( elements ) {}

	const elements_t elements;

	~Array() {
		for ( auto& it : elements ) {
			delete it;
		}
	}

	const std::string ToString() const override {
		std::string args = "";
		for ( const auto& it : elements ) {
			if ( !args.empty() ) {
				args += ", ";
			}
			args += it->ToString();
		}
		return "[ " + args + " ]";
	}
	const std::string Dump( const size_t depth = 0 ) const override {
		std::string result = Formatted( "Array" + m_si.ToString() + "( ", depth );
		for ( const auto& it : elements ) {
			result += Formatted( it->Dump(), depth + 1 );
		}
		return result + Formatted( ")", depth );
	}
};

}
}

