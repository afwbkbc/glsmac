#pragma once

#include <mutex>

#include "Logger.h"

namespace logger {

CLASS( Stdout, Logger )
	
#ifdef DEBUG
	void Log( const std::string &text );
	mutex m_log_mutex;
#else
	void Log( const std::string &text ) {}
#endif
	
};

} /* namespace logger */
