#include "GSE.h"

#include "parser/JS.h"
#include "runner/Interpreter.h"
#include "GlobalContext.h"
#include "Exception.h"
#include "type/Undefined.h"

#include "util/FS.h"

namespace gse {

GSE::GSE() {
	m_bindings.push_back( &m_builtins );
}

GSE::~GSE() {
	for ( auto& it : m_modules ) {
		DELETE( it.second );
	}
	for ( auto& it : m_include_cache ) {
		it.second.Cleanup();
	}
}

parser::Parser* GSE::GetParser( const std::string& filename, const std::string& source, const size_t initial_line_num ) const {
	parser::Parser* parser = nullptr;
	const auto extensions = util::FS::GetExtensions( filename );
	ASSERT( extensions.size() == 2 && extensions[ 0 ] == ".gls", "unsupported file name ( " + filename + " ), expected: *.gls.*" );
	if ( extensions[ 1 ] == ".js" ) {
		NEW( parser, parser::JS, filename, source, initial_line_num );
	}
	ASSERT( parser, "could not find parser for '.gls" + extensions[ 1 ] + "' extension" );
	return parser;
}

runner::Runner* GSE::GetRunner() const {
	NEWV( runner, runner::Interpreter );
	return runner;
}

void GSE::AddBindings( Bindings* bindings ) {
	m_bindings.push_back( bindings );
}

GlobalContext* GSE::CreateGlobalContext( const std::string& source_path ) {
	NEWV( context, gse::GlobalContext, this, source_path );
	for ( const auto& it : m_bindings ) {
		it->AddToContext( context );
	}
	return context;
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
		GlobalContext context( this, {} );
		it->second->Run( &context, {}, {} );
		// TODO: cleanup context properly or don't
	}

	Log( "GSE finished" );
}

const Value GSE::GetInclude( Context* ctx, const si_t& si, const std::string& path ) {
	const auto& it = m_include_cache.find( path );
	if ( it != m_include_cache.end() ) {
		return it->second.result;
	}
	std::string full_path = "";
	for ( const auto& it : m_supported_extensions ) {
		if ( util::FS::FileExists( path + it ) ) {
			if ( !full_path.empty() ) {
				throw gse::Exception( EC.LOADER_ERROR, "Multiple candidates found for include '" + path + "': '" + std::string( full_path ) + "', '" + path + it + "', ...", ctx, si );
			}
			full_path = path + it;
		}
	}
	if ( full_path.empty() ) {
		throw gse::Exception( EC.LOADER_ERROR, "Could not find script for include '" + path + "'", ctx, si );
	}
	const auto source = util::FS::ReadFile( full_path );
	include_cache_t cache = {
		VALUE( type::Undefined ),
		nullptr,
		nullptr,
		nullptr
	};
	try {
		cache.context = CreateGlobalContext( full_path );
		cache.context->IncRefs();
#ifdef DEBUG
		// copy mocks
		if ( ctx->HasVariable( "test" ) ) {
			cache.context->CreateVariable( "test", ctx->GetVariable( "test", &si ), &si );
		}
#endif
		const auto parser = GetParser( full_path, source );
		cache.program = parser->Parse();
		DELETE( parser );
		cache.runner = GetRunner();
		cache.context->IncRefs();
		cache.result = cache.runner->Execute( cache.context, cache.program );
		cache.context->DecRefs();
		m_include_cache.insert_or_assign( path, cache );
		return cache.result;
	}
	catch ( gse::Exception& e ) {
		cache.Cleanup();
		throw e;
	}
	catch ( std::runtime_error& e ) {
		cache.Cleanup();
		throw e;
	}
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

void GSE::include_cache_t::Cleanup() {
	{
		if ( context ) {
			context->DecRefs();
			context = nullptr;
		}
		if ( program ) {
			DELETE( program );
			program = nullptr;
		}
		if ( runner ) {
			DELETE( runner );
			runner = nullptr;
		}
	}
}

}
