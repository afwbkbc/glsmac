#include <cstdio>

#include "StdoutLogger.h"

namespace logger {

void StdoutLogger::Log( const std::string &text ) const {
	printf("%s\n", text.c_str());
}

} /* namespace logger */
