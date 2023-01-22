#include "Win32.h"

namespace error_handler {

void Win32::HandleError( const std::runtime_error &e ) const {
#ifdef _WIN32
	MessageBoxA( NULL, e.what() , "Application error", MB_OK );
	exit( EXIT_FAILURE );
#endif
}

} /* namespace error_handler */
