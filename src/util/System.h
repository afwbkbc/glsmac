#pragma once

#include <vector>
#include <string>

#include "Util.h"

namespace util {

CLASS( System, Util )

#if defined( DEBUG ) || defined( FASTDEBUG )

	static bool AreWeUnderGDB();
	static bool IsGDBAvailable();

#endif

};

}
