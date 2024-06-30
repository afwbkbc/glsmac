#pragma once

#include <string>

#include "common/Module.h"

namespace logger {

CLASS( Logger, common::Module )
	virtual void Log( const std::string& text ) = 0;
};

}
