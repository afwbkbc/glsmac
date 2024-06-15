#pragma once

#include <stdexcept>

#include "common/Module.h"

namespace error_handler {

CLASS( ErrorHandler, common::Module )
	virtual void HandleError( const std::runtime_error& e ) const = 0;
};

}
