#include "GSE.h"

#include "mocks/Mocks.h"

#include "util/FS.h"
#include "util/String.h"

#include "engine/Engine.h"

namespace gse {
namespace tests {

using namespace program;

#define TEST_SCRIPTS_PATH "gse/tests"

void AddScriptsTests( task::gsetests::GSETests* task ) {

	const std::string tests_path = "./" TEST_SCRIPTS_PATH;

	task->AddTest(
		"check if " + tests_path + " directory exists",
		GT( tests_path ) {
			if ( !util::FS::DirectoryExists( tests_path ) ) {
				GT_FAIL( "directory " + tests_path + " does not exist" );
			}
			GT_OK();
		}
	);

	const auto* c = g_engine->GetConfig();
	const auto scripts = c->HasDebugFlag( config::Config::DF_GSE_TESTS_SCRIPT )
		? std::vector< std::string >{ c->GetGSETestsScript() }
		: util::FS::ListDirectory( tests_path, true );
	for ( const auto& script : scripts ) {
		task->AddTest(
			"testing " + script,
			GT( task, script ) {

				gse::parser::Parser* parser = nullptr;
				const gse::runner::Runner* runner = nullptr;
				const gse::program::Program* program = nullptr;
				gse::Context* context = nullptr;

				std::string last_error = "";
				try {
					const auto source = util::FS::ReadFile( script );
					parser = gse.GetParser( script, source );
					NEW( context, gse::Context, nullptr, util::String::SplitToLines( source ), {} );
					mocks::AddMocks( context );
					program = parser->Parse();
					runner = gse.GetRunner();
					runner->Execute( context, program );
				}
				catch ( gse::Exception& e ) {
					last_error = e.ToStringAndCleanup();
				}
				catch ( std::runtime_error const& e ) {
					last_error = (std::string)"Internal error: " + e.what();
				};

				if ( context ) {
					DELETE( context );
				}
				if ( program ) {
					DELETE( program );
				}
				if ( runner ) {
					DELETE( runner );
				}
				if ( parser ) {
					DELETE( parser );
				}

				return last_error;
			}
		);
	}
}

}
}
