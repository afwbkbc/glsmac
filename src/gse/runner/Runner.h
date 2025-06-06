#pragma once

#include "gc/Object.h"

namespace gc {
class Space;
}

namespace gse {

class ExecutionPointer;
class Value;

namespace context {
class Context;
}

namespace program {
class Program;
}

namespace runner {

CLASS( Runner, gc::Object )

	Runner( gc::Space* const gc_space );

	virtual Value* const Execute( context::Context* ctx, ExecutionPointer& ep, const program::Program* program ) = 0;

#if defined( DEBUG ) || defined( FASTDEBUG )

	void EnableScopeContextJoins() {
		// for prompt, to remember previously entered commands
		m_are_scope_context_joins_enabled = true;
	}

#endif

protected:

	gc::Space* const m_gc_space = nullptr;

#if defined( DEBUG ) || defined( FASTDEBUG )
	bool m_are_scope_context_joins_enabled = false;
#endif

};

}
}
