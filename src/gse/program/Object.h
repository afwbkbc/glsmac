#pragma once

#include <string>
#include <unordered_map>

#include "Operand.h"

#include "Expression.h"

namespace gse {
namespace program {

class Object : public Operand {
public:

	Object( const std::unordered_map< std::string, Operand* >& properties )
		: Operand( OT_OBJECT )
		, properties( properties ) {}

	const std::unordered_map< std::string, Operand* > properties;

	~Object() {
		for ( auto& it : properties ) {
			delete it.second;
		}
	}
};

}
}

