#pragma once

#include <string>

#include "Util.h"

namespace util {

CLASS( SMACChecker, Util )
	static bool IsSMACDirectory( const std::string& path );

private:
};

}
