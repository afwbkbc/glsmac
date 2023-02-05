#pragma once

#include "Util.h"

namespace util {
	
CLASS( FS, Util )
	
	static const bool FileExists( const string& filename );

	static const string ReadFile( const string& filename );
	static const void WriteFile( const string& filename, const string& data );
	
};

}
