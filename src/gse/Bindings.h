#pragma once

namespace gse {

class ExecutionPointer;

namespace context {
class Context;
}

class Bindings {
public:
	virtual void AddToContext( context::Context* ctx, ExecutionPointer& ep ) = 0;
};

}
