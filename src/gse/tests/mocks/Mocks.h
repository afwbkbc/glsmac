#pragma once

#include "gse/tests/Types.h"

namespace gse {

class GlobalContext;

namespace tests {
namespace mocks {

void AddMocks( gse::GlobalContext* ctx, const test_info_t& test_info );

}
}
}
