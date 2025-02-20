#pragma once

namespace gc {
class Space;
}

namespace gse {

class ExecutionPointer;

namespace context {
class Context;
}

class Bindings {
public:
	virtual void AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) = 0;
};

}
