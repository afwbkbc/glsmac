#include "Try.h"

#include "Scope.h"
#include "Catch.h"

namespace gse {
namespace program {

Try::Try( const si_t& si, const Scope* body, const Catch* handlers )
	: Conditional( si, CT_TRY )
	, body( body )
	, handlers( handlers ) {}

Try::~Try() {
	delete body;
	delete handlers;
}

}
}
