#include "GSE.h"

#include "parser/JS.h"
#include "runner/Interpreter.h"
#include "gse/context/GlobalContext.h"
#include "Exception.h"
#include "value/Undefined.h"
#include "program/Program.h"
#include "util/FS.h"
#include "gc/Space.h"
#include "Async.h"
#include "ExecutionPointer.h"

namespace gse {

// scripts must be cross-platform
const char GSE::PATH_SEPARATOR = '/';

GSE::GSE()
	: gc::Object( nullptr ) {
	NEW( m_gc_space, gc::Space, this );
	m_bindings.push_back( &m_builtins );
	m_gc_space->Accumulate(
		[ this ]() {
			m_async = new Async( m_gc_space );
		}
	);
}

GSE::~GSE() {
	{
		std::lock_guard guard( m_gc_mutex );
		Finish();
		for ( auto& it : m_include_cache ) {
			it.second.Cleanup( this );
		}

		// make everything unreachable so that gc could clean it
		m_global_contexts.clear();
		m_parsers.clear();
		m_runner = nullptr;
		m_async = nullptr;
		m_modules.clear();
		m_root_objects.clear();
	}
	delete m_gc_space;
}

void GSE::Iterate() {
	ExecutionPointer ep;
	try {
		m_async->Iterate( ep );
	}
	catch ( const Exception& e ) {
		Log( e.ToString() );
		throw;
	}
}

void GSE::Finish() {
	{
		ExecutionPointer ep;
		m_async->ProcessAndExit( ep );
	}
}

parser::Parser* GSE::CreateParser( const std::string& filename, const std::string& source, const size_t initial_line_num ) {
	parser::Parser* parser = nullptr;
	const auto extensions = util::FS::GetExtensions( filename, PATH_SEPARATOR );
	ASSERT( extensions.size() == 2 && extensions.at( 0 ) == ".gls", "unsupported file name ( " + filename + " ), expected: *.gls.*" );
	const auto& ext = extensions.at( 1 );

	if ( ext == ".js" ) {
		NEW( parser, parser::JS, m_gc_space, filename, source, initial_line_num );
	}

	ASSERT( parser, "could not find parser for '.gls" + ext + "' extension" );
	return parser;
}

parser::Parser* GSE::GetParser( const std::string& filename, const std::string& source, const size_t initial_line_num ) {
	parser::Parser* parser = nullptr;
	{
		std::lock_guard guard( m_gc_mutex );
		auto it = m_parsers.find( filename );
		if ( it == m_parsers.end() ) {
			m_gc_space->Accumulate(
				[ this, &parser, &filename, &source, &initial_line_num, &it ]() {
					parser = CreateParser( filename, source, initial_line_num );
					it = m_parsers.insert(
						{
							filename,
							parser
						}
					).first;
				}
			);
		}
		return it->second;
	}
}

runner::Runner* GSE::GetRunner() {
	std::lock_guard guard( m_gc_mutex );
	if ( !m_runner ) {
		m_gc_space->Accumulate(
			[ this ]() {
				NEW( m_runner, runner::Interpreter, m_gc_space );
			}
		);
	}
	return m_runner;
}

void GSE::AddBindings( Bindings* bindings ) {
	m_bindings.push_back( bindings );
}

context::GlobalContext* GSE::CreateGlobalContext( const std::string& source_path ) {
	context::GlobalContext* context;
	m_gc_space->Accumulate(
		[ this, &context, &source_path ]() {
			NEW( context, context::GlobalContext, this, source_path );
			{
				//std::lock_guard guard( m_gc_mutex );
				m_global_contexts.insert( context );
			}
			{
				gse::ExecutionPointer ep;
				for ( const auto& it : m_bindings ) {
					it->AddToContext( m_gc_space, context, ep );
				}
			}
		}
	);
	return context;
}

void GSE::AddModule( const std::string& path, value::Callable* module ) {
	std::lock_guard guard( m_gc_mutex );
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
	std::unordered_map< std::string, value::Callable* >::const_iterator it;
	for ( auto& i : m_modules_order ) {
		{
			std::lock_guard guard( m_gc_mutex );
			it = m_modules.find( i );
			ASSERT( it != m_modules.end(), "required module missing: " + i );
		}
		Log( "Executing module: " + it->first );
		auto* context = CreateGlobalContext();
		m_gc_space->Accumulate(
			[ this, &it, &context ]() {
				ExecutionPointer ep;
				it->second->Run( m_gc_space, context, {}, ep, {} );
			}
		);
	}

	Log( "GSE finished" );
}

Value* const GSE::RunScript( GSE_CALLABLE, const std::string& path ) {
	CHECKACCUM( m_gc_space );
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
						GSE_ERROR( EC.LOADER_ERROR, "Multiple candidates found for include '" + path + "': '" + std::string( full_path ) + "', '" + path + it2 + "', ..." );
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
		GSE_ERROR( EC.LOADER_ERROR, "Could not find script for include '" + path + "'" );
	}
	const auto& it = m_include_cache.find( full_path );
	if ( it != m_include_cache.end() ) {
		return it->second.result;
	}
	const auto source = util::FS::ReadTextFile( full_path, PATH_SEPARATOR );
	include_cache_t cache = {
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};
	try {
		cache.context = CreateGlobalContext( full_path );
#if defined( DEBUG ) || defined( FASTDEBUG )
		// copy mocks
		if ( ctx && ctx->HasVariable( "test" ) ) {
			cache.context->CreateVariable( "test", ctx->GetVariable( "test", si, ep ), si, ep );
		}
#endif
		const auto parser = GetParser( full_path, source );
		cache.program = parser->Parse();
		cache.runner = GetRunner();
		{
			cache.result = cache.runner->Execute( cache.context, ep, cache.program );
		}
		m_include_cache.insert_or_assign( full_path, cache );
		return cache.result;
	}
	catch ( const Exception& e ) {
		cache.Cleanup( this );
		Log( e.ToString() );
		throw;
	}
	catch ( const std::runtime_error& e ) {
		cache.Cleanup( this );
		throw;
	}
}

void GSE::SetGlobal( const std::string& identifier, Value* variable ) {
	ASSERT( m_globals.find( identifier ) == m_globals.end(), "duplicate global: " + identifier );
	Log( "Set global: " + identifier );
	m_globals.insert_or_assign( identifier, variable );
}

Value* const GSE::GetGlobal( const std::string& identifier ) {
	Log( "Get global: " + identifier );
	CHECKACCUM( m_gc_space );
	const auto& it = m_globals.find( identifier );
	if ( it != m_globals.end() ) {
		return it->second;
	}
	else {
		return VALUEEXT( value::Undefined, m_gc_space );
	}
}

void GSE::AddRootObject( gc::Object* const object ) {
	std::lock_guard guard( m_gc_mutex );
	ASSERT_NOLOG( m_root_objects.find( object ) == m_root_objects.end(), "root object already exists" );
	m_root_objects.insert( object );
}

void GSE::RemoveRootObject( gc::Object* const object ) {
	std::lock_guard guard( m_gc_mutex );
	ASSERT_NOLOG( m_root_objects.find( object ) != m_root_objects.end(), "root object not found" );
	m_root_objects.erase( object );
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

gc::Space* const GSE::GetGCSpace() const {
	ASSERT_NOLOG( m_gc_space, "gse gc space not set" );
	return m_gc_space;
}

void GSE::GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) {
	std::lock_guard guard( m_gc_mutex );

	GC_DEBUG_BEGIN( "runner" );
	if ( m_runner ) {
		GC_REACHABLE( m_runner );
	}
	GC_DEBUG_END();

	// parsers are reachable
	GC_DEBUG_BEGIN( "parsers" );
	for ( const auto& it : m_parsers ) {
		GC_REACHABLE( it.second );
	}
	GC_DEBUG_END();

	// global contexts are reachable
	GC_DEBUG_BEGIN( "global_contexts" );
	for ( const auto& context : m_global_contexts ) {
		GC_REACHABLE( context );
	}
	GC_DEBUG_END();

	GC_DEBUG_BEGIN( "async" );
	if ( m_async ) {
		GC_REACHABLE( m_async );
	}
	GC_DEBUG_END();

	// modules are reachable
	GC_DEBUG_BEGIN( "modules" );
	for ( const auto& it : m_modules ) {
		GC_REACHABLE( it.second );
	}
	GC_DEBUG_END();

	// root objects are reachable
	GC_DEBUG_BEGIN( "root_objects" );
	for ( const auto& object : m_root_objects ) {
		GC_REACHABLE( object );
	}
	GC_DEBUG_END();

}

#if defined ( DEBUG ) || defined( FASTDEBUG )
const std::string GSE::ToString() {
	return "gse::GSE()";
}
#endif

void GSE::include_cache_t::Cleanup( GSE* const gse ) {
	{
		if ( context ) {
			context->Clear();
			context = nullptr;
		}
		if ( program ) {
			DELETE( program );
			program = nullptr;
		}
		runner = nullptr;
		result = nullptr;
	}
}

}
