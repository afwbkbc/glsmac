#include "StdoutErrorHandler.h"

namespace error_handler {

void StdoutErrorHandler::HandleError( const base::Error &e ) const {
	printf( "FATAL ERROR: %s\n", e.what() );
	exit( EXIT_FAILURE );
}

} /* namespace error_handler */
