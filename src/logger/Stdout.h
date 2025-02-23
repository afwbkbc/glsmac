#pragma once

#include <atomic>

#include "Logger.h"

namespace logger {

// hacky but ok
extern std::atomic< bool > g_is_muted;

CLASS( Stdout, Logger )

	void Log( const std::string& text ) override;

};

}
