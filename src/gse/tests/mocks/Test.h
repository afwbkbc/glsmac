#pragma once

#include "gse/tests/Types.h"

namespace gc {
class Space;
}

namespace gse {

namespace context {
class GlobalContext;
}

namespace tests {
namespace mocks {

class Test {
public:
	static void AddMocks( gc::Space* const gc_space, context::GlobalContext* ctx, const test_info_t& test_info );

};

}
}
}
