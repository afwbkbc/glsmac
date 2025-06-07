#include "Test.h"

#include <unordered_map>
#include <chrono>

#include "gse/context/GlobalContext.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/value/Undefined.h"
#include "gse/value/Object.h"
#include "gse/value/Bool.h"
#include "gse/value/Int.h"
#include "gse/value/String.h"
#include "gse/tests/Tests.h"
#include "gse/ExecutionPointer.h"

namespace gse {
namespace tests {
namespace mocks {

static std::unordered_map< std::string, Value* > s_global_map = {};

void Test::AddMocks( gc::Space* const gc_space, context::GlobalContext* ctx, const test_info_t& test_info ) {
	value::object_properties_t mocks = {
		{
			"assert",
			NATIVE_CALL() {
				if ( arguments.size() < 1 ) {
					GSE_ERROR( EC.INVALID_CALL, "Condition to test.assert is missing" );
				}
				if ( arguments.size() > 2 ) {
					GSE_ERROR( EC.INVALID_CALL, "Expected 1 or 2 arguments to test.assert, found: " + std::to_string( arguments.size() ) );
				}
				const auto& condition = arguments.at( 0 );
				if ( condition->type != Value::T_BOOL ) {
					GSE_ERROR( EC.INVALID_CALL, "Condition to test.assert is not boolean: " + condition->ToString() );
				}
				if ( !( (value::Bool*)condition )->value ) {
					std::string reason = "";
					if ( arguments.size() == 2 ) {
						const auto& reasonv = arguments.at( 1 );
						ASSERT( reasonv->type == Value::T_STRING, "test.assert reason is not string: " + reasonv->ToString() );
						reason = ( (value::String*)reasonv )->value;
					}
					GSE_ERROR( "TestError", "Assertion failed: " + reason );
				}
				return VALUE( value::Undefined );
			} )
		},
		{
			"get_script_path",
			NATIVE_CALL( test_info ) {
				return VALUE( value::String,, test_info.script_path );
			} )
		},
		{
			"get_current_time_nano",
			NATIVE_CALL() {

				return VALUE( value::Int,, std::chrono::time_point_cast< std::chrono::nanoseconds >( std::chrono::system_clock::now() ).time_since_epoch().count() );
			} )
		},
		{
			"global_get",
			NATIVE_CALL() {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( key, 0, String );
				const auto& it = s_global_map.find( key );
				if ( it != s_global_map.end() ) {
					return it->second;
				}
				else {
					return VALUE( value::Undefined );
				}
			} )
		},
		{
			"global_set",
			NATIVE_CALL() {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( key, 0, String );
				N_GET( value, 1 );
				s_global_map.insert_or_assign( key, value );
				return VALUE( value::Undefined );
			} )
		},
	};
	{
		ExecutionPointer ep;
		ctx->CreateVariable( "test", VALUE( value::Object,, ctx, {}, ep, mocks ), {}, ep );
	}
}

}
}
}
