#pragma once

#include "Util.h"

namespace util {

CHILD_CLASS( System, Util )

#if DEBUG
	static bool AreWeUnderGDB();
	static bool IsGDBAvailable();
#endif

};

}
