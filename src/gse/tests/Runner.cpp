#include "GSE.h"

#include "Tests.h"

#include "gse/Context.h"

#include "gse/type/Undefined.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"

#include "gse/runner/Interpreter.h"

#include "mocks/Mocks.h"
#include "mocks/Console.h"

#include "util/String.h"

namespace gse {
namespace tests {

using namespace program;

void AddRunnerTests( task::gsetests::GSETests* task ) {

	const auto& test_program = GetTestProgram();

	const std::string expected_output = GetExpectedResult();

#define VALIDATE() { \
    GT_ASSERT( actual_output == expected_output, ", actual output:\n" + actual_output ); \
}
	task->AddTest(
		"test if interpreter executes programs correctly",
		GT( task, test_program, expected_output ) {

			runner::Interpreter interpreter;

			Context context( nullptr, util::String::SplitToLines( GetTestSource() ), {} );
			mocks::AddMocks( &context );

			interpreter.Execute( &context, test_program );

			// ugh
			const auto& console = context.GetVariable( "console", nullptr );
			GT_ASSERT( console.Get()->type == type::Type::T_OBJECT, "console mock mismatch" );
			const auto& log = ( (type::Object*)console.Get() )->Get( "log" );
			GT_ASSERT( log.Get()->type == type::Type::T_CALLABLE, "console.log mock mismatch" );
			const auto actual_output = mocks::Console::GetLogsOf( (type::Callable*)log.Get() );

			VALIDATE();

			GT_OK();
		}
	);

}

}
}
