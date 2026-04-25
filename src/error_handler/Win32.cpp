#include "Win32.h"

#ifdef _WIN32
#include "util/LogHelper.h"
#endif

namespace error_handler {

void Win32::HandleError( const std::runtime_error& e ) const {
#ifdef _WIN32
	util::LogHelper::Println( e.what() );
	MessageBoxA( NULL, e.what() , "Application error", MB_OK );
	exit( EXIT_FAILURE );
#endif
}

}
