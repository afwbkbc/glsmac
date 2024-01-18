#include "Test.h"

#include "gse/type/Object.h"
#include "gse/type/Bool.h"
#include "gse/type/Int.h"
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
		},
		{
			"get_current_time_nano",
			VALUE( GetCurrentTimeNano )
		}
	};
	ctx->CreateVariable( "test", VALUE( type::Object, mocks ), nullptr );
}

Value Test::Assert::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {
	if ( arguments.size() < 1 ) {
		throw gse::Exception( EC.INVALID_CALL, "Condition to test.assert is missing", ctx, call_si );
	}
	if ( arguments.size() > 2 ) {
		throw gse::Exception( EC.INVALID_CALL, "Expected 1 or 2 arguments to test.assert, found: " + std::to_string( arguments.size() ), ctx, call_si );
	}
	const auto& condition = arguments.at( 0 );
	if ( condition.Get()->type != type::Type::T_BOOL ) {
		throw gse::Exception( EC.INVALID_CALL, "Condition to test.assert is not boolean: " + condition.ToString(), ctx, call_si );
	}
	if ( !( (type::Bool*)condition.Get() )->value ) {
		std::string reason = "";
		if ( arguments.size() == 2 ) {
			const auto& reasonv = arguments.at( 1 );
			ASSERT_NOLOG( reasonv.Get()->type == type::Type::T_STRING, "test.assert reason is not string: " + reasonv.ToString() );
			reason = ( (type::String*)reasonv.Get() )->value;
		}
		throw gse::Exception( "TestError", "Assertion failed: " + reason, ctx, call_si );
	}
	return VALUE( type::Undefined );
}

Test::GetScriptPath::GetScriptPath( const std::string& path )
	: m_path( path ) {

}

Value Test::GetScriptPath::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {
	return VALUE( type::String, m_path );
}

Value Test::GetCurrentTimeNano::Run( Context* ctx, const si_t& call_si, const Callable::function_arguments_t& arguments ) {
	return VALUE( type::Int, std::chrono::time_point_cast< std::chrono::nanoseconds >( std::chrono::system_clock::now() ).time_since_epoch().count() );
}

}
}
}
