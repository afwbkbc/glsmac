#pragma once

#include <string>

#include "base/Module.h"

namespace config {

CLASS( Config, base::Module )
	Config( const int argc, const char *argv[] );
	
	enum launch_flag_t : uint8_t {
		LF_NONE = 0,
		LF_BENCHMARK = 1 << 0
	};

	const std::string& GetSMACPath() const;
	
	const bool HasLaunchFlag( launch_flag_t flag ) const;
	
private:
	const int m_argc = 0;
	const char** const m_argv = nullptr;
	
	std::string m_smac_path = "";
	
	uint8_t m_launch_flags = LF_NONE;
};

} /* namespace config */
