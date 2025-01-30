#include "GSE.h"

#include "parser/JS.h"
#include "runner/Interpreter.h"
#include "gse/context/GlobalContext.h"
#include "Exception.h"
#include "type/Undefined.h"
#include "program/Program.h"
#include "util/FS.h"
#include "Async.h"
#include "ExecutionPointer.h"

namespace gse {

// scripts must be cross-platform
const char GSE::PATH_SEPARATOR = '/';

GSE::GSE() {
	m_bindings.push_back( &m_builtins );
	m_async = new Async();
}

GSE::~GSE() {
	{
		ExecutionPointer ep;
		m_async->ProcessAndExit( ep );
	}
	delete m_async;
	for ( auto& it : m_modules ) {
		DELETE( it.second );
	}
	for ( auto& it : m_include_cache ) {
		it.second.Cleanup( true );
	}
}

void GSE::Iterate() {
	ExecutionPointer ep;
	m_async->Iterate( ep );
}

parser::Parser* GSE::GetParser( const std::string& filename, const std::string& source, const size_t initial_line_num ) const {
	parser::Parser* parser = nullptr;
	const auto extensions = util::FS::GetExtensions( filename, PATH_SEPARATOR );
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

context::GlobalContext* GSE::CreateGlobalContext( const std::string& source_path ) {
	NEWV( context, context::GlobalContext, this, source_path );
	{
		gse::ExecutionPointer ep;
		for ( const auto& it : m_bindings ) {
			it->AddToContext( context, ep );
		}
	}
	return context;
}

void GSE::AddModule( const std::string& path, type::Callable* module ) {
	if ( m_modules.find( path ) != m_modules.end() ) {
		ExecutionPointer ep;
		throw Exception( "GSE_InternalError", "module path '" + path + "' already taken", nullptr, {}, ep ); // ?
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
		context::GlobalContext context( this, {} );
		{
			ExecutionPointer ep;
			it->second->Run( &context, {}, ep, {} );
		}
	}

	Log( "GSE finished" );
}

const Value GSE::RunScript( GSE_CALLABLE, const std::string& path ) {
	std::string full_path = "";
	{
		const auto& it = m_include_paths.find( path );
		if ( it != m_include_paths.end() ) {
			full_path = it->second;
		}
		else {
			for ( const auto& it2 : m_supported_extensions ) {
				if ( util::FS::FileExists( path + it2, PATH_SEPARATOR ) ) {
					if ( !full_path.empty() ) {
						throw Exception( EC.LOADER_ERROR, "Multiple candidates found for include '" + path + "': '" + std::string( full_path ) + "', '" + path + it2 + "', ...", GSE_CALL );
					}
					full_path = path + it2;
				}
			}
			m_include_paths.insert(
				{
					path,
					full_path
				}
			);
		}
	}
	if ( full_path.empty() ) {
		Log( "Could not find script for include '" + path + "'" );
		throw Exception( EC.LOADER_ERROR, "Could not find script for include '" + path + "'", GSE_CALL );
	}
	const auto& it = m_include_cache.find( full_path );
	if ( it != m_include_cache.end() ) {
		return it->second.result;
	}
	const auto source = util::FS::ReadTextFile( full_path, PATH_SEPARATOR );
	include_cache_t cache = {
		VALUE( type::Undefined ),
		nullptr,
		nullptr,
		nullptr
	};
	try {
		cache.context = CreateGlobalContext( full_path );
#ifdef DEBUG
		// copy mocks
		if ( ctx && ctx->HasVariable( "test" ) ) {
			cache.context->CreateVariable( "test", ctx->GetVariable( "test", si, ep ), si, ep );
		}
#endif
		cache.context->Begin();
		const auto parser = GetParser( full_path, source );
		cache.program = parser->Parse();
		DELETE( parser );
		cache.runner = GetRunner();
		{
			ExecutionPointer ep;
			cache.result = cache.runner->Execute( cache.context, ep, cache.program );
		}
		m_include_cache.insert_or_assign( full_path, cache );
		return cache.result;
	}
	catch ( std::runtime_error& e ) {
		cache.Cleanup();
		throw;
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

context::Context* GSE::GetContextByPath( const std::string& path ) const {
	const auto& it = m_include_cache.find( path );
	if ( it != m_include_cache.end() ) {
		return it->second.context;
	}
	return nullptr;
}

Async* GSE::GetAsync() {
	return m_async;
}

void GSE::include_cache_t::Cleanup( const bool force ) {
	{
		if ( force ) {
			result = VALUE( type::Undefined );
		}
		if ( context ) {
			if ( !force ) {
				if ( context->End() ) {
					context = nullptr;
				}
			}
			else {
				context->Clear();
				context->DecRefs();
				context = nullptr;
			}
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
