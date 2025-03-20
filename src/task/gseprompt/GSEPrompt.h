#pragma once

#include "common/Task.h"
#include "gc/Object.h"

namespace gc {
class Space;
}

namespace gse {
class GSE;

namespace context {
class GlobalContext;
}

namespace parser {
class Parser;
}

namespace runner {
class Runner;
}
namespace program {
class Program;
}
}

namespace task {
namespace gseprompt {

CLASS2( GSEPrompt, common::Task, gc::Object )

	GSEPrompt( const std::string& syntax );
	~GSEPrompt();

	void Start() override;
	void Stop() override;
	void Iterate() override;

	void GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) override;
#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString();
#endif

private:
	bool m_is_running = false;

	const std::string m_syntax;

	const bool m_is_tty;

	gse::GSE* m_gse = nullptr;
	gc::Space* m_gc_space = nullptr;
	gse::parser::Parser* m_parser = nullptr;
	gse::context::GlobalContext* m_gse_context = nullptr;
	gse::runner::Runner* m_runner;

	fd_set fds = {};
	int maxfd = 0;
	fd_set rfds = {};
	struct timeval tv = {};
	int retval = 0, len = 0;
	char buff[255] = { 0 };

	size_t m_lines_count = 0;
	std::string m_input = "";

	void PrintPrompt();
	void ProcessInput();

	std::vector< const gse::program::Program* > m_programs = {};

};

}
}
