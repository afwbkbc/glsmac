#pragma once

#include <string>

#include "base/Module.h"

namespace logger {

MAJOR_CLASS( Logger, base::Module )
	virtual void Log( const std::string &text ) const = 0;
};

} /* namespace logger */
