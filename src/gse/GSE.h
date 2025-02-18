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

namespace type {
class Callable;
}

class Async;

CLASS( GSE, common::Class )
	GSE();
	virtual ~GSE();

	static const char PATH_SEPARATOR;

	void Iterate();

	parser::Parser* GetParser( const std::string& filename, const std::string& source, const size_t initial_line_num = 1 ) const;
	runner::Runner* GetRunner() const;

	void AddBindings( Bindings* bindings );

	context::GlobalContext* CreateGlobalContext( const std::string& source_path = "" );

	void AddModule( const std::string& path, type::Callable* module );

	void Run();
	const Value RunScript( GSE_CALLABLE, const std::string& path );

	void SetGlobal( const std::string& identifier, Value variable );
	const Value& GetGlobal( const std::string& identifier );

	context::Context* GetContextByPath( const std::string& path ) const;

	Async* GetAsync();

	gc::Space* const GetGCSpace() const;

#if defined ( DEBUG ) || defined( FASTDEBUG )

	void LogCaptureStart() const { m_builtins.LogCaptureStart(); }
	const std::string& LogCaptureStopGet() const { return m_builtins.LogCaptureStopGet(); }

#endif

private:

	gc::Space* m_gc_space = nullptr;
	std::unordered_set< context::GlobalContext* > m_global_contexts = {};

	const std::unordered_set< std::string > m_supported_extensions = {
		".gls.js",
	};

	std::unordered_map< std::string, type::Callable* > m_modules = {};
	std::vector< std::string > m_modules_order = {};
	std::map< std::string, Value > m_globals = {};

	std::vector< Bindings* > m_bindings = {};
	builtins::Builtins m_builtins = {};

	struct include_cache_t {
		Value result;
		context::GlobalContext* context;
		// TODO: why can't we delete these two upon getting result?
		const program::Program* program;
		const runner::Runner* runner;
		void Cleanup( GSE* const gse );
	};
	std::unordered_map< std::string, std::string > m_include_paths = {};
	std::unordered_map< std::string, include_cache_t > m_include_cache = {};

	Async* m_async = nullptr;
};

}
