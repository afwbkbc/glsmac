#pragma once

#include <string>

#include "base/Module.h"

namespace logger {

CLASS( Logger, base::Module )
	virtual void Log( const std::string& text ) = 0;
};

}
