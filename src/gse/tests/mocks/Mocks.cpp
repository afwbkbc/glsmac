#include "Mocks.h"

#include "Test.h"

#include "gc/Space.h"

namespace gse {
namespace tests {
namespace mocks {

void AddMocks( gc::Space* const gc_space, context::GlobalContext* ctx, const test_info_t& test_info ) {
	gc_space->Accumulate(
		[ &gc_space, &ctx, &test_info ]() {
			Test::AddMocks( gc_space, ctx, test_info );
		}
	);
}

}
}
}
