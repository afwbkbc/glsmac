#pragma once

#include "ErrorHandler.h"

namespace error_handler {

class StdoutErrorHandler : public ErrorHandler {
public:
	void HandleError( const base::Error &e ) const;
};

} /* namespace error_handler */
