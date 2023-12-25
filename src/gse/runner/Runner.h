#pragma once

#include "base/Base.h"

#include "gse/Context.h"
#include "gse/program/Program.h"

namespace gse {
namespace runner {

CLASS( Runner, base::Base )

	virtual const Value Execute( Context* ctx, const program::Program* program ) const = 0;

};

}
}
