#include "Mocks.h"

#include "Console.h"
#include "Test.h"

namespace gse {
namespace tests {
namespace mocks {

void AddMocks( gse::Context* ctx, const test_info_t& test_info ) {
	Test::AddMocks( ctx, test_info );
	Console::AddMocks( ctx );
}

}
}
}
