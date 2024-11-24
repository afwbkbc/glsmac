#pragma once

#include "Logger.h"

namespace logger {

CLASS( Console, Logger )

	void Log( const std::string& text ) override;

};

}
