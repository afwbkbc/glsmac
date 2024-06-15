#include "Win32.h"

#include <iostream>

namespace error_handler {

void Win32::HandleError( const std::runtime_error& e ) const {
#ifdef _WIN32
	std::cout << e.what() << std::endl;
	MessageBoxA( NULL, e.what() , "Application error", MB_OK );
	exit( EXIT_FAILURE );
#endif
}

}
