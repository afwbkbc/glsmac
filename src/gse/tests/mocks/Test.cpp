#include "Test.h"

#include <unordered_map>
#include <chrono>

#include "gse/context/GlobalContext.h"
#include "gse/callable/Native.h"
#include "gse/Exception.h"
#include "gse/type/Undefined.h"
#include "gse/type/Object.h"
#include "gse/type/Bool.h"
#include "gse/type/Int.h"
#include "gse/type/String.h"
#include "gse/tests/Tests.h"

namespace gse {
namespace tests {
namespace mocks {

static std::unordered_map< std::string, Value > s_global_map = {};

void Test::AddMocks( context::GlobalContext* ctx, const test_info_t& test_info ) {
	type::object_properties_t mocks = {
		{
			"assert",
			NATIVE_CALL() {
				if ( arguments.size() < 1 ) {
					throw Exception( EC.INVALID_CALL, "Condition to test.assert is missing", GSE_CALL );
				}
				if ( arguments.size() > 2 ) {
					throw Exception( EC.INVALID_CALL, "Expected 1 or 2 arguments to test.assert, found: " + std::to_string( arguments.size() ), GSE_CALL );
				}
				const auto& condition = arguments.at( 0 );
				if ( condition.Get()->type != type::Type::T_BOOL ) {
					throw Exception( EC.INVALID_CALL, "Condition to test.assert is not boolean: " + condition.ToString(), GSE_CALL );
				}
				if ( !( (type::Bool*)condition.Get() )->value ) {
					std::string reason = "";
					if ( arguments.size() == 2 ) {
						const auto& reasonv = arguments.at( 1 );
						ASSERT_NOLOG( reasonv.Get()->type == type::Type::T_STRING, "test.assert reason is not string: " + reasonv.ToString() );
						reason = ( (type::String*)reasonv.Get() )->value;
					}
					throw Exception( "TestError", "Assertion failed: " + reason, GSE_CALL );
				}
				return VALUE( type::Undefined );
			} )
		},
		{
			"get_script_path",
			NATIVE_CALL( test_info ) {
				return VALUE( type::String, test_info.script_path );
			} )
		},
		{
			"get_current_time_nano",
			NATIVE_CALL() {
				return VALUE( type::Int, std::chrono::time_point_cast< std::chrono::nanoseconds >( std::chrono::system_clock::now() ).time_since_epoch().count() );
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
					return VALUE( type::Undefined );
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
				return VALUE( type::Undefined );
			} )
		},
	};
	ctx->CreateVariable( "test", VALUE( type::Object, nullptr, mocks ), nullptr );
}

}
}
}
