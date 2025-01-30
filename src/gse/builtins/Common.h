#pragma once

#include "gse/Bindings.h"

namespace gse {
namespace builtins {

class Common : public Bindings {
public:
	void AddToContext( context::Context* ctx, ExecutionPointer& ep ) override;
};

}
}
