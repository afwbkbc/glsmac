#include "GSETests.h"

#include "gse/GSE.h"
#include "gse/Async.h"
#include "gse/tests/Tests.h"
#include "engine/Engine.h"
#include "gse/program/Program.h"
#include "util/LogHelper.h"
#include "gse/context/GlobalContext.h"
#include "gc/Space.h"

namespace gse::tests {
const gse::program::Program* GetTestProgram( gc::Space* const gc_space );
}

namespace task {
namespace gsetests {

void GSETests::Start() {
	gse::tests::AddTests( this );
}

void GSETests::Stop() {
	if ( m_stats.failed == 0 ) {
		LogTest( "All tests passed." );
	}
	else {
		LogTest( "Testing complete, passed: " + std::to_string( m_stats.passed ) + ", failed: " + std::to_string( m_stats.failed ) );
	}
	m_tests.clear();
	m_stats.passed = 0;
	m_stats.failed = 0;
}

void GSETests::Iterate() {
	if ( current_test_index < m_tests.size() ) {
		const auto& it = m_tests[ current_test_index++ ];
		LogTest( "  " + it.first + "..." );
		gse::GSE gse;
		gse::context::GlobalContext* ctx;
		gse.GetGCSpace()->Accumulate(
			nullptr,
			[ &gse, &ctx ]() {
				ctx = gse.CreateGlobalContext();
			}
		);
		const auto errmsg = it.second( &gse, ctx );
		if ( errmsg.empty() ) {
			m_stats.passed++;
		}
		else {
			m_stats.failed++;
			LogTest( "    !!! TEST FAILED: " + errmsg );
		}
	}
	else if ( current_test_index == m_tests.size() ) {
		current_test_index++;
		g_engine->ShutDown();
	}
}

void GSETests::AddTest( const std::string& name, const gse_test_t test ) {
	m_tests.push_back(
		{
			name,
			test
		}
	);
}

void GSETests::LogTest( const std::string& text, bool is_debug ) {
	if ( is_debug ) {
		Log( text );
	}
	else {
		util::LogHelper::Println( text );
	}
}

}
}
