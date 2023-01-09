#pragma once

#include "Logger.h"

namespace logger {

class StdoutLogger : public Logger {
public:
	void Log( const std::string &text ) const;
};

} /* namespace logger */
