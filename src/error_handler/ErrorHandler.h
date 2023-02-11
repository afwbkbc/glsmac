#pragma once

#include <stdexcept>

#include "base/Module.h"

namespace error_handler {

CLASS( ErrorHandler, base::Module )
	virtual void HandleError( const runtime_error &e ) const = 0;
};

} /* namespace error_handler */
