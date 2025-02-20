#pragma once

#include "gse/Bindings.h"

namespace gse {

namespace builtins {

class String : public Bindings {
public:
	void AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) override;
};

}
}
