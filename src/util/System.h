#pragma once

#include "Util.h"

namespace util {

CLASS( System, Util )

#ifdef DEBUG
	static bool AreWeUnderGDB();
	static bool IsGDBAvailable();
#endif

};

}
