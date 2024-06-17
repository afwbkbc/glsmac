#pragma once

#include "common/Common.h"

#include "gse/Value.h"

namespace gse {

namespace context {
class Context;
}

namespace program {
class Program;
}

namespace runner {

CLASS( Runner, common::Class )

	virtual const Value Execute( context::Context* ctx, const program::Program* program ) const = 0;

#ifdef DEBUG
	void EnableScopeContextJoins() {
		// for prompt, to remember previously entered commands
		m_are_scope_context_joins_enabled = true;
	}
#endif

protected:
#ifdef DEBUG
	bool m_are_scope_context_joins_enabled = false;
#endif

};

}
}
