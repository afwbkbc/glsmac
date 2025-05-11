#pragma once

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <vector>
#include <functional>

#include "common/Common.h"

#include "Types.h"

#include "Value.h"
#include "builtins/Builtins.h"

namespace gc {
class Space;
}

namespace gse {

namespace context {
class GlobalContext;
}

class Bindings;

namespace parser {
class Parser;
}

namespace runner {
class Runner;
}

namespace program {
class Program;
}

namespace value {
class Callable;
}

class Async;

CLASS2( GSE, common::Class, gc::Object )
	GSE();
	virtual ~GSE();

	static const char PATH_SEPARATOR;

	void Iterate();
	void Finish();

	parser::Parser* CreateParser( const std::string& filename, const std::string& source, const size_t initial_line_num = 1 );
	parser::Parser* GetParser( const std::string& filename, const std::string& source, const size_t initial_line_num = 1 );
	runner::Runner* GetRunner();

	void AddBindings( Bindings* bindings );

	context::GlobalContext* CreateGlobalContext( const std::string& source_path = "" );

	void AddModule( const std::string& path, value::Callable* module );

	void Run();
	Value* const RunScript( GSE_CALLABLE, const std::string& path );

	void SetGlobal( const std::string& identifier, Value* variable );
	Value* const GetGlobal( const std::string& identifier );

	void AddRootObject( gc::Object* const object );
	void RemoveRootObject( gc::Object* const object );

	context::Context* GetContextByPath( const std::string& path ) const;
	Async* GetAsync();
	gc::Space* const GetGCSpace() const;

	void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) override;

#if defined ( DEBUG ) || defined( FASTDEBUG )

	void LogCaptureStart() const { m_builtins.LogCaptureStart(); }
	const std::string& LogCaptureStopGet() const { return m_builtins.LogCaptureStopGet(); }

#endif

private:

	gc::Space* m_gc_space = nullptr;
	std::unordered_set< context::GlobalContext* > m_global_contexts = {};
	std::unordered_map< std::string, parser::Parser* > m_parsers = {}; // extension, parser
	runner::Runner* m_runner = nullptr;

	const std::unordered_set< std::string > m_supported_extensions = {
		".gls.js",
	};

	std::unordered_map< std::string, value::Callable* > m_modules = {};
	std::vector< std::string > m_modules_order = {};
	std::map< std::string, Value* > m_globals = {};

	std::unordered_set< gc::Object* > m_root_objects = {};

	std::vector< Bindings* > m_bindings = {};
	builtins::Builtins m_builtins = {};

	struct include_cache_t {
		Value* result;
		context::GlobalContext* context;
		// TODO: why can't we delete these two upon getting result?
		const program::Program* program;
		runner::Runner* runner;
		void Cleanup( GSE* const gse );
	};
	std::unordered_map< std::string, std::string > m_include_paths = {};
	std::unordered_map< std::string, include_cache_t > m_include_cache = {};

	Async* m_async = nullptr;
};

}
