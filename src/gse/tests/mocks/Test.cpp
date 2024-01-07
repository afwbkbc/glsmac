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
	ASSERT_NOLOG( arguments.size() == 2, "test.assert expects exactly 2 arguments, " + std::to_string( arguments.size() ) + " found" );
	const auto& condition = arguments.at( 0 );
	ASSERT_NOLOG( condition.Get()->type == type::Type::T_BOOL, "test.assert condition is not boolean: " + condition.ToString() );
	const auto& reason = arguments.at( 1 );
	ASSERT_NOLOG( reason.Get()->type == type::Type::T_STRING, "test.assert reason is not string: " + reason.ToString() );
	if ( !( (type::Bool*)condition.Get() )->value ) {
		throw gse::Exception( "TestError", "assertion failed: " + ( (type::String*)reason.Get() )->value, ctx, call_si );
	}
	return VALUE( type::Undefined );
}

}
}
}
