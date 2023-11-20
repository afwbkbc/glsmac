#include "WorldTests.h"

#include "world/World.h"
#include "world/Tests.h"
#include "engine/Engine.h"

namespace task {
namespace worldtests {

void WorldTests::Start() {
	Log( "Loading tests" );
	world::AddTests( this );
}

void WorldTests::Stop() {
	if ( m_stats.failed == 0 ) {
		Log( "All tests passed." );
	}
	else {
		Log( "Testing complete, passed: " + std::to_string( m_stats.passed ) + ", failed: " + std::to_string( m_stats.failed ) );
	}
	m_tests.clear();
	m_stats.passed = 0;
	m_stats.failed = 0;
}

void WorldTests::Iterate() {
	if ( current_test_index < m_tests.size() ) {
		world::World world;
		const auto& it = m_tests[ current_test_index++ ];
		Log( it.first + "..." );
		const auto errmsg = it.second( world );
		if ( errmsg.empty() ) {
			m_stats.passed++;
			Log( "      ... OK" );
		}
		else {
			m_stats.failed++;
			Log( "      ... " + errmsg );
		}
	}
	else if ( current_test_index == m_tests.size() ) {
		current_test_index++;
		g_engine->ShutDown();
	}
}

void WorldTests::AddTest( const std::string& name, const world_test_t test ) {
	m_tests.push_back(
		{
			name,
			test
		}
	);
}

}
}
