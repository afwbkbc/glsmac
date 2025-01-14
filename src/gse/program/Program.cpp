#include "Program.h"

#include "Scope.h"

namespace gse {
namespace program {

Program::Program( const Scope* body )
	: body( body ) {}
Program::~Program() {
	//delete body; // ?
}

const std::string Program::Dump() const {
	return "Program(\n" + body->Dump( 1 ) + ")\n";
}

}
}