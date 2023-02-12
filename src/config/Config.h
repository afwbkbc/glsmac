#pragma once

#include <string>

#include "base/Module.h"

namespace config {

CLASS( Config, base::Module )
	Config( const int argc, const char *argv[] );
	
	const std::string& GetSMACPath() {
		return m_smac_path;
	}
	
private:
	const int m_argc = 0;
	const char** const m_argv = nullptr;
	
	std::string m_smac_path = "";
};

} /* namespace config */
