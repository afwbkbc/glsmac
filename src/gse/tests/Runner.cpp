#include "GSE.h"

#include "Tests.h"

#include "gse/ChildContext.h"

#include "gse/type/Undefined.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"
#include "gse/type/String.h"
#include "gse/type/Object.h"

#include "gse/runner/Interpreter.h"

#include "mocks/Mocks.h"

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

			GlobalContext* context = gse.CreateGlobalContext();
			context->IncRefs();
			context->AddSourceLines( util::String::SplitToLines( GetTestSource() ) );
			mocks::AddMocks( context, {} );

			gse.LogCaptureStart();
			interpreter.Execute( context, test_program );
			const auto actual_output = gse.LogCaptureStopGet();

			VALIDATE();

			context->DecRefs();

			GT_OK();
		}
	);

}

}
}
