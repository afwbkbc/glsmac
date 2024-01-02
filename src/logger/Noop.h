#pragma once

#include "Logger.h"

namespace logger {

CLASS( Noop, Logger )

	void Log( const std::string& text ) override {}

};

}
