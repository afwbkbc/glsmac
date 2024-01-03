#include "GSE.h"

#include "parser/GJS.h"
#include "runner/Interpreter.h"
#include "util/FS.h"

#include "builtin/Builtins.h"

#include "type/Undefined.h"

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

parser::Parser* GSE::GetParser( const std::string& filename, const std::string& source, const size_t initial_line_num ) const {
	parser::Parser* parser = nullptr;
	const auto extension = util::FS::GetExtension( filename );
	if ( extension == ".gjs" ) {
		NEW( parser, parser::GJS, filename, source, initial_line_num );
	}
	ASSERT( parser, "could not find parser for '" + extension + "' extension" );
	return parser;
}

const runner::Runner* GSE::GetRunner() const {
	NEWV( runner, runner::Interpreter );
	return runner;
}

void GSE::AddModule( const std::string& path, type::Callable* module ) {
	if ( m_modules.find( path ) != m_modules.end() ) {
		throw Exception( "GSE_InternalError", "module path '" + path + "' already taken", nullptr, {} ); // ?
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
		Context context( nullptr, {}, {} );
		it->second->Run( &context, {}, {} );
		// TODO: cleanup context properly or don't
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

}
