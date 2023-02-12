#pragma once

#include "Util.h"

namespace util {
	
CLASS( FS, Util )
	
	static const bool FileExists( const std::string& filename );

	static void CreateDirectoryIfNotExists( const std::string& path );

	static const std::string ReadFile( const std::string& filename );
	static const void WriteFile( const std::string& filename, const std::string& data );
	
};

}
