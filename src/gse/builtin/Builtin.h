#pragma once

#include "gse/callable/Native.h"

#include "gse/Context.h"

namespace gse {
namespace builtin {

class Builtin {
public:
	virtual void AddToContext( gse::Context* ctx ) const = 0;
};

}
}
