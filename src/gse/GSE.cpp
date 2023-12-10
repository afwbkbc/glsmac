#include "GSE.h"

#include "parser/GJS.h"

#include "builtin/Builtins.h"

#include "type/Undefined.h"

namespace gse {

GSE::GSE() {

	NEWV( gjs, parser::GJS );
	AddParser( gjs );

	NEWV( builtins, builtin::Builtins );
	AddModule( "_", builtins );
}

GSE::~GSE() {
	for ( auto& it : m_modules ) {
		DELETE( it.second );
	}
	for ( auto& it : m_parsers ) {
		DELETE( it );
	}
}

void GSE::AddModule( const std::string& path, type::Callable* module ) {
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
		it->second->Run( this, {} );
	}

	Log( "GSE finished" );
}

void GSE::SetGlobal( const std::string& identifier, Value variable ) {
	ASSERT( m_globals.find( identifier ) == m_globals.end(), "duplicate global: " + identifier );
	Log( "Set global: " + identifier );
	m_globals.insert_or_assign( identifier, variable );
}

const static Value s_undefined_value = VALUE( type::Undefined );
const Value& GSE::GetGlobal( const std::string& identifier ) {
	Log( "Get global: " + identifier );
	const auto& it = m_globals.find( identifier );
	if ( it != m_globals.end() ) {
		return it->second;
	}
	else {
		return s_undefined_value;
	}
}

void GSE::AddParser( parser::Parser* parser ) {
	for ( const auto& it : parser->GetSupportedExtensions() ) {
		ASSERT( m_extensions.find( it ) == m_extensions.end(), "multiple parsers found for extension: " + it );
		Log( "Using parser " + parser->GetParserName() + " for extension '" + it + "'" );
		m_extensions.insert_or_assign( it, parser );
	}
	m_parsers.push_back( parser );
}

}
