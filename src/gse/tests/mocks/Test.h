#pragma once

#include "Mocks.h"

namespace gse {
namespace tests {
namespace mocks {

class Test {
public:

	static void AddMocks( gse::Context* ctx, const test_info_t& test_info );

private:

	class Assert : public type::Callable {
	public:
		Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;
	};

	class GetScriptPath : public type::Callable {
	public:
		GetScriptPath( const std::string& path );
		Value Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) override;
	private:
		const std::string m_path;
	};

};

}
}
}

