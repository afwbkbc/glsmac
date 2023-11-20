#include "Exception.h"

namespace gse {

Exception::Exception( const std::string& errmsg )
	: std::runtime_error( errmsg ) {
	// nothing
}

}