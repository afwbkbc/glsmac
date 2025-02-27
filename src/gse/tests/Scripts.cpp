#include "GSE.h"

#include "mocks/Mocks.h"

#include "gse/GSE.h"
#include "gse/Exception.h"
#include "gse/context/GlobalContext.h"
#include "gse/parser/Parser.h"
#include "gse/runner/Runner.h"
#include "gse/program/Program.h"
#include "gse/Async.h"
#include "config/Config.h"
#include "task/gsetests/GSETests.h"
#include "util/FS.h"
#include "util/String.h"
#include "gse/ExecutionPointer.h"
#include "gc/Space.h"
#include "engine/Engine.h"

namespace gse {
namespace tests {

void AddScriptsTests( task::gsetests::GSETests* task ) {

	const std::string tests_path = util::FS::GeneratePath(
		{
			".",
			"GLSMAC_data",
			"tests",
		}, GSE::PATH_SEPARATOR
	);

	const auto* c = g_engine->GetConfig();
	if ( !c->HasDebugFlag( config::Config::DF_GSE_TESTS_SCRIPT ) ) {
		task->AddTest(
			"check if " + tests_path + " directory exists",
			GT( tests_path ) {
				if ( !util::FS::DirectoryExists( tests_path, GSE::PATH_SEPARATOR ) ) {
					GT_FAIL( "directory " + tests_path + " does not exist" );
				}
				GT_OK();
			}
		);
	}
	const auto scripts = c->HasDebugFlag( config::Config::DF_GSE_TESTS_SCRIPT )
		? std::vector< std::string >{ c->GetGSETestsScript() }
		: util::FS::ListDirectory( tests_path, true, GSE::PATH_SEPARATOR );
	for ( const auto& script : scripts ) {
		if ( script.substr( 0, tests_path.size() + 2 ) == tests_path + GSE::PATH_SEPARATOR + "_" ) {
			continue; // these should not be tested directly (i.e. includes)
		}
		task->AddTest(
			"testing " + script,
			GT( script ) {
				auto* gc_space = gse->GetGCSpace();

				parser::Parser* parser = nullptr;
				const runner::Runner* runner = nullptr;
				const program::Program* program = nullptr;
				context::GlobalContext* context = nullptr;

				std::string last_error = "";
				try {
					const auto source = util::FS::ReadTextFile( script, GSE::PATH_SEPARATOR );
					parser = gse->GetParser( script, source );
					context = gse->CreateGlobalContext( script );
					mocks::AddMocks( gc_space, context, { script } );
					program = parser->Parse();
					runner = gse->GetRunner();
					{
						ExecutionPointer ep;
						gc_space->Accumulate(
							[ &runner, &context, &ep, &program ]() {
								runner->Execute( context, ep, program );
							}
						);
					}
				}
				catch ( Exception& e ) {
					last_error = e.ToString();
					if ( context ) {
						context = nullptr;
					}
				}
				catch ( std::runtime_error const& e ) {
					last_error = (std::string)"Internal error: " + e.what();
				};

				gse->Finish();

				if ( program ) {
					DELETE( program );
				}

				return last_error;
			}
		);
	}
}

}
}
