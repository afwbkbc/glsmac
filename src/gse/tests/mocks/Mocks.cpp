#include "Mocks.h"

#include "Console.h"
#include "Test.h"

namespace gse {
namespace tests {
namespace mocks {

void AddMocks( gse::Context* ctx ) {
	Test::AddMocks( ctx );
	Console::AddMocks( ctx );
}

}
}
}
