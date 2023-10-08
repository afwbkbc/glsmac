#pragma once

#include <uuid.h>

#include "Util.h"

namespace util {

CLASS( UUID, Util )

	static const std::string Generate( unsigned int mode = UUID_MAKE_V4 );

};

}
