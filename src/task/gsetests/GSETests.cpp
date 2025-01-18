#include <iostream> // not using Log() everywhere because important stuff should be printed with --quiet too

#include "GSETests.h"

#include "gse/GSE.h"
#include "gse/Async.h"
#include "gse/tests/Tests.h"
#include "engine/Engine.h"
#include "gse/program/Program.h"
#include "gse/tests/Tests.h"

namespace gse::tests {
const gse::program::Program* GetTestProgram();
const gse::program::Program* g_test_program = nullptr;
}

namespace task {
namespace gsetests {

void GSETests::Start() {
	ASSERT( !gse::tests::g_test_program, "test program already set" );
	gse::tests::g_test_program = gse::tests::GetTestProgram();
	Log( "Loading tests" );
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
	ASSERT( gse::tests::g_test_program, "test program not set" );
	delete gse::tests::g_test_program;
	gse::tests::g_test_program = nullptr;
}

void GSETests::Iterate() {
	if ( current_test_index < m_tests.size() ) {
		gse::GSE gse;
		const auto& it = m_tests[ current_test_index++ ];
		LogTest( "  " + it.first + "..." );
		const auto errmsg = it.second( &gse );
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
		std::cout << text << std::endl;
	}
}

}
}
