#include "GSE.h"

#include "Tests.h"

#include "task/gsetests/GSETests.h"
#include "gse/GSE.h"
#include "gse/context/GlobalContext.h"
#include "gse/runner/Interpreter.h"
#include "gse/ExecutionPointer.h"
#include "gse/program/Program.h"

#include "mocks/Mocks.h"

#include "util/String.h"

namespace gse {
namespace tests {

void AddRunnerTests( task::gsetests::GSETests* task ) {

	const std::string expected_output = GetExpectedResult();

#define VALIDATE() { \
    GT_ASSERT( actual_output == expected_output, ", actual output:\n" + actual_output ); \
}
	task->AddTest(
		"test if interpreter executes programs correctly",
		GT( expected_output ) {
			auto* gc_space = gse->GetGCSpace();

			runner::Interpreter interpreter( gc_space );

			context::GlobalContext* context = gse->CreateGlobalContext();
			context->AddSourceLines( util::String::Split( GetTestSource(), '\n' ) );
			mocks::AddMocks( gc_space, context, {} );

			gse->LogCaptureStart();
			const auto* test_program = gse::tests::GetTestProgram( gse->GetGCSpace() );
			ASSERT_NOLOG( test_program, "test program is null" );
			try {
				ExecutionPointer ep;
				interpreter.Execute( context, ep, test_program );
			}
			catch ( const std::runtime_error& e ) {
				delete test_program;
				context->Clear();
				throw;
			}
			delete test_program;
			context->Clear();

			const auto actual_output = gse->LogCaptureStopGet();

			VALIDATE();

			GT_OK();
		}
	);

}

}
}
