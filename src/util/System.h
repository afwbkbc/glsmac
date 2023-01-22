#pragma once

#include "Util.h"

namespace util {

CLASS( System, Util )

#if DEBUG
	static bool AreWeUnderGDB();
	static bool IsGDBAvailable();
#endif

};

}
