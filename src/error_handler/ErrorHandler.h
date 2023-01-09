#pragma once

#include <base/Error.h>
#include <base/Module.h>

namespace error_handler {

MAJOR_CLASS( ErrorHandler, base::Module )
	virtual void HandleError( const base::Error &e ) const = 0;
};

} /* namespace error_handler */
