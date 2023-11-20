#include "GSE.h"

#include "builtin/Builtins.h"

namespace gse {

GSE::GSE() {
	NEWV( builtins, builtin::Builtins );
	AddModule( "_", builtins );
}

GSE::~GSE() {
	for ( auto& it : m_modules ) {
		DELETE( it.second );
	}
}

void GSE::AddModule( const std::string& path, runnable::Runnable* module ) {
	if ( m_modules.find( path ) != m_modules.end() ) {
		throw Exception( "module path '" + path + "' already taken" );
	}
	Log( "Adding module: " + path );
	m_modules[ path ] = module;
	m_modules_order.push_back( path );
}

void GSE::Run() {
	Log( "GSE started" );

	// execute all modules in loading order
	for ( auto& i : m_modules_order ) {
		auto it = m_modules.find( i );
		ASSERT( it != m_modules.end(), "required module missing: " + i );
		Log( "Executing module: " + it->first );
		it->second->Run();
	}

	Log( "GSE finished" );
}

}
