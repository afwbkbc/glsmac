#include "Program.h"

#include "Scope.h"

namespace gse {
namespace program {

Program::Program( const Scope* body, const bool is_body_owned )
	: body( body )
	, m_is_body_owned( is_body_owned ) {}

Program::~Program() {
	if ( m_is_body_owned ) {
		delete body;
	}
}

const std::string Program::Dump() const {
	return "Program(\n" + body->Dump( 1 ) + ")\n";
}

}
}