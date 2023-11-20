#include "WorldTests.h"

#include "world/Tests.h"
#include "engine/Engine.h"

namespace task {
namespace worldtests {

void WorldTests::Start() {
	Log( "Loading tests" );
	world::AddTests( this );
}

void WorldTests::Stop() {
	Log( "Exiting" );
}

void WorldTests::Iterate() {
	if ( current_test_index < m_tests.size() ) {
		const auto& it = m_tests[ current_test_index++ ];
		Log( it.first + "..." );
		const auto errmsg = it.second();
		if ( errmsg.empty() ) {
			Log( "      ... OK" );
		}
		else {
			Log( "      ... Failed: " + errmsg );
		}
	}
	else if ( current_test_index == m_tests.size() ) {
		current_test_index++;
		Log( "Tests finished" );
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
