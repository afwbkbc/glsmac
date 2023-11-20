#pragma once

#include <unordered_map>
#include <vector>

#include "base/Base.h"

#include "runnable/Runnable.h"
#include "Exceptions.h"

namespace world {

CLASS( World, base::Base )
	World();
	virtual ~World();

	void AddModule( const std::string& path, runnable::Runnable* module );

	void Run();

private:
	std::unordered_map< std::string, runnable::Runnable* > m_modules = {};
	std::vector< std::string > m_modules_order = {};

};

}
