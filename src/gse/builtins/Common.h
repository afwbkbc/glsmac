#pragma once

#include "gse/Bindings.h"

namespace gse {
namespace builtins {

class Common : public Bindings {
public:
	void AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) override;
};

}
}
