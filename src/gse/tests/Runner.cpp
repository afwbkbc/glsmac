#include "GSE.h"

#include "Tests.h"

#include "task/gsetests/GSETests.h"
#include "gse/GSE.h"
#include "gse/context/GlobalContext.h"
#include "gse/runner/Interpreter.h"
#include "gse/ExecutionPointer.h"

#include "mocks/Mocks.h"

#include "util/String.h"

namespace gse {
namespace tests {

extern const program::Program* g_test_program;

void AddRunnerTests( gc::Space* const gc_space, task::gsetests::GSETests* task ) {

	const std::string expected_output = GetExpectedResult();

#define VALIDATE() { \
    GT_ASSERT( actual_output == expected_output, ", actual output:\n" + actual_output ); \
}
	task->AddTest(
		"test if interpreter executes programs correctly",
		GT( &gc_space, expected_output ) {

			runner::Interpreter interpreter( gc_space );

			context::GlobalContext* context = gse->CreateGlobalContext();
			context->AddSourceLines( util::String::Split( GetTestSource(), '\n' ) );
			mocks::AddMocks( gc_space, context, {} );

			gse->LogCaptureStart();
			try {
				ExecutionPointer ep;
				interpreter.Execute( context, ep, g_test_program );
			}
			catch ( const std::runtime_error& e ) {
				context->Clear();
				throw;
			}
			context->Clear();

			const auto actual_output = gse->LogCaptureStopGet();

			VALIDATE();

			GT_OK();
		}
	);

}

}
}
