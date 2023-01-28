#pragma once

#include "Util.h"

#include <vector>
#include <string>

namespace util {

CLASS( System, Util )

#ifdef DEBUG
	static bool AreWeUnderGDB();
	static bool IsGDBAvailable();
#endif

	// to workaround uppercase or lowercase filenames
	static vector< string > GetPossibleFilenames( const string& filename );
	
};

}
