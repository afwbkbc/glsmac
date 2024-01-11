#include "Test.h"

#include "gse/type/Object.h"
#include "gse/type/Bool.h"
#include "gse/type/String.h"
#include "gse/type/Undefined.h"

#include "gse/Exception.h"
#include "gse/tests/Tests.h"

namespace gse {
namespace tests {
namespace mocks {

void Test::AddMocks( gse::Context* ctx, const test_info_t& test_info ) {
	type::Object::properties_t mocks = {
		{
			"assert",
			VALUE( Assert )
		},
		{
			"get_script_path",
			VALUE( GetScriptPath, test_info.script_path )
		}
	};
	ctx->CreateVariable( "test", VALUE( type::Object, mocks ), nullptr );
}

Value Test::Assert::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {
	ASSERT_NOLOG( arguments.size() >= 1, "test.assert argument missing" );
	ASSERT_NOLOG( arguments.size() <= 2, "test.assert got too many arguments (expected 1 or 2, found " + std::to_string( arguments.size() ) + ")" );
	const auto& condition = arguments.at( 0 );
	ASSERT_NOLOG( condition.Get()->type == type::Type::T_BOOL, "test.assert condition is not boolean: " + condition.ToString() );
	if ( !( (type::Bool*)condition.Get() )->value ) {
		std::string reason = "";
		if ( arguments.size() == 2 ) {
			const auto& reasonv = arguments.at( 1 );
			ASSERT_NOLOG( reasonv.Get()->type == type::Type::T_STRING, "test.assert reason is not string: " + reasonv.ToString() );
			reason = ( (type::String*)reasonv.Get() )->value;
		}
		throw gse::Exception( "TestError", "assertion failed: " + reason, ctx, call_si );
	}
	return VALUE( type::Undefined );
}

Test::GetScriptPath::GetScriptPath( const std::string& path )
	: m_path( path ) {

}

Value Test::GetScriptPath::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {
	return VALUE( type::String, m_path );
}

}
}
}
