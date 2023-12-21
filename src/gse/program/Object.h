#pragma once

#include <string>
#include <unordered_map>

#include "Operand.h"
#include "Expression.h"

namespace gse {
namespace program {

class Object : public Operand {
public:

	Object( const std::unordered_map< std::string, const Expression* >& properties )
		: Operand( OT_OBJECT )
		, properties( properties ) {}

	const std::unordered_map< std::string, const Expression* > properties;

	~Object() {
		for ( auto& it : properties ) {
			delete it.second;
		}
	}

	const std::string ToString( const size_t depth ) const override {
		std::string result = Formatted( "Object( ", depth );
		for ( const auto& it : properties ) {
			result +=
				Formatted( it.first + ":", depth + 1 ) +
					it.second->ToString( depth + 2 );
		}
		return result +
			Formatted( ")", depth );
	}
};

}
}

