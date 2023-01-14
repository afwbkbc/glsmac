#include <cstdio>

#include "StdoutLogger.h"

namespace logger {

#if DEBUG
void StdoutLogger::Log( const std::string &text ) const {
	printf("%s\n", text.c_str());
	fflush( stdout );
}
#endif

} /* namespace logger */
