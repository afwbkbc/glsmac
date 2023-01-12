#pragma once

#include "Logger.h"

namespace logger {

class StdoutLogger : public Logger {
public:
	
#if DEBUG
	void Log( const std::string &text ) const;
#else
	void Log( const std::string &text ) const {}
#endif
	
};

} /* namespace logger */
