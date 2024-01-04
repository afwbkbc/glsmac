#pragma once

#include <unordered_map>
#include <map>
#include <vector>

#include "base/Base.h"

#include "type/Type.h"
#include "gse/type/Callable.h"
#include "Exception.h"

#include "program/Program.h"

namespace gse {

namespace parser {
class Parser;
}

namespace runner {
class Runner;
}

CLASS( GSE, base::Base )
	GSE();
	virtual ~GSE();

	parser::Parser* GetParser( const std::string& filename, const std::string& source, const size_t initial_line_num = 1 ) const;
	const runner::Runner* GetRunner() const;

	void AddModule( const std::string& path, type::Callable* module );

	void Run();

	void SetGlobal( const std::string& identifier, Value variable );
	const Value& GetGlobal( const std::string& identifier );

private:

	std::unordered_map< std::string, type::Callable* > m_modules = {};
	std::vector< std::string > m_modules_order = {};
	std::map< std::string, Value > m_globals = {};

};

}

#include "parser/Parser.h"
#include "runner/Runner.h"
