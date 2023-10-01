#pragma once

#include <vector>
#include <string>

#include "Util.h"

namespace util {

CLASS( System, Util )

#ifdef DEBUG

	static bool AreWeUnderGDB();
	static bool IsGDBAvailable();

#endif

	// to workaround uppercase or lowercase filenames
	static std::vector< std::string > GetPossibleFilenames( const std::string& filename );

};

}
