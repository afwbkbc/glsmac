#pragma once

#include "Mocks.h"

namespace gse {
namespace tests {
namespace mocks {

class Test {
public:

	static void AddMocks( gse::Context* ctx );

private:

	class Assert : public type::Callable {
		Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;
	};

};

}
}
}

