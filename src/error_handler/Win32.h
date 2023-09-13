#pragma once

#include "ErrorHandler.h"

namespace error_handler {

CLASS( Win32, ErrorHandler )
	void HandleError( const std::runtime_error &e ) const override;
};

} /* namespace error_handler */
