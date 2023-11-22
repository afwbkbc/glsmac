#pragma once

#include <unordered_map>
#include <map>
#include <vector>

#include "base/Base.h"

#include "type/Type.h"
#include "runnable/Runnable.h"
#include "Exception.h"

namespace gse {

namespace builtin {
class Builtins;
}

CLASS( GSE, base::Base )
	GSE();
	virtual ~GSE();

	void AddModule( const std::string& path, runnable::Runnable* module );

	void Run();

private:
	friend class builtin::Builtins;
	void RegisterGlobal( const std::string& identifier, Value variable );
	std::map< std::string, Value > m_globals = {};

private:

	std::unordered_map< std::string, runnable::Runnable* > m_modules = {};
	std::vector< std::string > m_modules_order = {};

};

}
