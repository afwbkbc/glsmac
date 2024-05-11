#pragma once

#include "base/Base.h"

#include "gse/Value.h"

namespace gse {

class Context;

namespace program {
class Program;
}

namespace runner {

CLASS( Runner, base::Base )

	virtual const Value Execute( Context* ctx, const program::Program* program ) const = 0;

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
