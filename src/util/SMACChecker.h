#pragma once

#include "Util.h"

#include <string>

namespace util {

CHILD_CLASS( SMACChecker, Util )
	static void CheckPath( const std::string& path );

private:
};

} /* namespace util */
