#include "Exceptions.h"

namespace world {

Exception::Exception( const std::string& errmsg )
	: std::runtime_error( errmsg ) {
	// nothing
}

}