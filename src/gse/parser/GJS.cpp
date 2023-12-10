#include "GJS.h"

#include "gse/type/Int.h"
#include "gse/type/String.h"

#include "gse/program/Variable.h"
#include "gse/program/Value.h"
#include "gse/program/Object.h"
#include "gse/program/Function.h"
#include "gse/program/Call.h"

namespace gse {
namespace parser {

using namespace program;

#define PARSER_SCOPE new program::Scope

const program::Program* GJS::Parse( const std::string& extension, const std::string& source ) const {

	NEWV( scope, program::Scope, {} );
	
	// TODO

	NEWV( program, program::Program, scope );
	return program;
}

}
}
