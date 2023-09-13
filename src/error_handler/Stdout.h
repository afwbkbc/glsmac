#pragma once

#include "ErrorHandler.h"

namespace error_handler {

CLASS( Stdout, ErrorHandler )
	void HandleError( const std::runtime_error &e ) const override;
};

} /* namespace error_handler */
