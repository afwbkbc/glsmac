#pragma once

#include "base/Base.h"

#include "gse/Value.h"
#include "gse/program/Program.h"

namespace gse {
class Context;
namespace runner {

CLASS( Runner, base::Base )

	virtual const Value Execute( Context* ctx, const program::Program* program ) const = 0;

};

}
}

#include "gse/Context.h"
