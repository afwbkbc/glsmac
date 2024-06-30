#pragma once

#include "gse/tests/Types.h"

namespace gse {

namespace context {
class GlobalContext;
}

namespace tests {
namespace mocks {

class Test {
public:
	static void AddMocks( context::GlobalContext* ctx, const test_info_t& test_info );

};

}
}
}
