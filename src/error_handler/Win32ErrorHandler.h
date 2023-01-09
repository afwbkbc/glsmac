#pragma once

#include "ErrorHandler.h"

namespace error_handler {

class Win32ErrorHandler : public ErrorHandler {
public:
	void HandleError( const base::Error &e ) const;
};

} /* namespace error_handler */
