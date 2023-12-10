#pragma once

#include <unordered_map>
#include <map>
#include <vector>

#include "base/Base.h"

#include "type/Type.h"
#include "gse/type/Callable.h"
#include "Exception.h"

namespace gse {

namespace parser {
class Parser;
}

CLASS( GSE, base::Base )
	GSE();
	virtual ~GSE();

	void AddModule( const std::string& path, type::Callable* module );

	void Run();

	void SetGlobal( const std::string& identifier, Value variable );
	const Value& GetGlobal( const std::string& identifier );

private:

	void AddParser( parser::Parser* parser );

	std::vector< parser::Parser* > m_parsers = {};
	std::unordered_map< std::string, parser::Parser* > m_extensions = {};

	std::unordered_map< std::string, type::Callable* > m_modules = {};
	std::vector< std::string > m_modules_order = {};
	std::map< std::string, Value > m_globals = {};

};

}

#include "parser/Parser.h"
