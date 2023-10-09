#pragma once

#ifndef _WIN32 // TODO: make ossp-uuid buildable on windows
#include <uuid.h>
#else
#define UUID_MAKE_V4 0
#endif

#include "Util.h"

namespace util {

CLASS( UUID, Util )

	static const std::string Generate( unsigned int mode = UUID_MAKE_V4 );

};

}
