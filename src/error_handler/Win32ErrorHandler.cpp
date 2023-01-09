#include "Win32ErrorHandler.h"

namespace error_handler {

void Win32ErrorHandler::HandleError( const base::Error &e ) const {
#ifdef _WIN32
	MessageBoxA( NULL, e.what() , "Application error", MB_OK );
	exit( EXIT_FAILURE );
#endif
}

} /* namespace error_handler */
