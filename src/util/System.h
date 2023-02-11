#pragma once

#include <vector>
#include <string>

#include "Util.h"

using namespace std;

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
