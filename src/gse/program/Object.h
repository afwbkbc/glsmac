#pragma once

#include <string>
#include <unordered_map>

#include "Operand.h"
#include "Expression.h"

namespace gse {
namespace program {

class Object : public Operand {
public:
	typedef std::unordered_map< std::string, const Expression* > properties_t;

	Object( const si_t& si, const properties_t& properties )
		: Operand( si, OT_OBJECT )
		, properties( properties ) {}

	const properties_t properties;

	~Object() {
		for ( auto& it : properties ) {
			delete it.second;
		}
	}

	const std::string ToString() const override {
		std::string args = "";
		for ( const auto& it : properties ) {
			if ( !args.empty() ) {
				args += ", ";
			}
			args += it.first + ": " + it.second->ToString();
		}
		return "{ " + args + " }";
	}
	const std::string Dump( const size_t depth = 0 ) const override {
		std::string result = Formatted( "Object" + m_si.ToString() + "( ", depth );
		for ( const auto& it : properties ) {
			result +=
				Formatted( it.first + ":", depth + 1 ) +
					it.second->Dump( depth + 2 );
		}
		return result +
			Formatted( ")", depth );
	}
};

}
}

