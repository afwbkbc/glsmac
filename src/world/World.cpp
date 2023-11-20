#include "World.h"

namespace world {

World::World() {

}

World::~World() {
	for ( auto& it : m_modules ) {
		DELETE( it.second );
	}
}

void World::AddModule( const std::string& path, runnable::Runnable* module ) {
	if ( m_modules.find( path ) != m_modules.end() ) {
		throw Exception( "module path '" + path + "' already taken" );
	}
	Log( "Adding module: " + path );
	m_modules[ path ] = module;
	m_modules_order.push_back( path );
}

void World::Run() {
	Log( "World started" );

	// execute all modules in loading order
	for ( auto& i : m_modules_order ) {
		auto it = m_modules.find( i );
		ASSERT( it != m_modules.end(), "required module missing: " + i );
		Log( "Executing module: " + it->first );
		it->second->Run();
	}

	Log( "World finished" );
}

}
