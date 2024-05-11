#pragma once

#include "gse/tests/Types.h"

namespace gse {

class GlobalContext;

namespace tests {
namespace mocks {

class Test {
public:
	static void AddMocks( gse::GlobalContext* ctx, const test_info_t& test_info );

};

}
}
}
