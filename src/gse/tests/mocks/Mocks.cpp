#include "Mocks.h"

#include "Test.h"

namespace gse {
namespace tests {
namespace mocks {

void AddMocks( gc::Space* const gc_space, context::GlobalContext* ctx, const test_info_t& test_info ) {
	Test::AddMocks( gc_space, ctx, test_info );
}

}
}
}
