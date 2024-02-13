#pragma once

#include "Scope.h"

namespace gse {
namespace program {

class Program {
public:

	Program( const Scope* body )
		: body( body ) {}

	const Scope* body;

	~Program() {
		delete body;
	}

	const std::string Dump() const {
		return "Program(\n" + body->Dump( 1 ) + ")\n";
	}

};

}
}
