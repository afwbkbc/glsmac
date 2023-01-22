#include "Stdout.h"

namespace error_handler {

void Stdout::HandleError( const std::runtime_error &e ) const {
	printf( "FATAL ERROR: %s\n", e.what() );
	//exit( EXIT_FAILURE );
	throw e;
}

} /* namespace error_handler */
