#pragma once

#include "gse/Bindings.h"

namespace gse {

namespace builtins {

class String : public Bindings {
public:
	void AddToContext( context::Context* ctx, ExecutionPointer& ep ) override;
};

}
}
