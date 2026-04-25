#pragma once

#include "gse/Bindings.h"

namespace util::random {
class Random;
}

namespace gse {
namespace builtins {

class Math : public Bindings {
public:
	Math();
	~Math();

	void AddToContext( gc::Space* const gc_space, context::Context* ctx, ExecutionPointer& ep ) override;

private:
	util::random::Random* m_random = nullptr;
};

}
}
