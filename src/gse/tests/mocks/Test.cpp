#include "Test.h"

#include "gse/type/Object.h"
#include "gse/type/Bool.h"
#include "gse/type/String.h"
#include "gse/type/Undefined.h"

#include "gse/Exception.h"

namespace gse {
namespace tests {
namespace mocks {

void Test::AddMocks( gse::Context* ctx ) {
	type::Object::properties_t mocks = {
		{
			"assert",
			VALUE( Assert )
		}
	};
	ctx->CreateVariable( "test", VALUE( type::Object, mocks ) );
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

}
}
}
