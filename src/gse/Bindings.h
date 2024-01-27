#pragma once

#include "gse/callable/Native.h"

#include "gse/ChildContext.h"

namespace gse {

class Bindings {
public:
	virtual void AddToContext( gse::Context* ctx ) const = 0;
};

}
