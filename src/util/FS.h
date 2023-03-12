#pragma once

#include "Util.h"

namespace util {
	
CLASS( FS, Util )
	
	static const bool Exists( const std::string& path );
	static const bool FileExists( const std::string& path );
	static const bool DirectoryExists( const std::string& path );

	static void CreateDirectoryIfNotExists( const std::string& path );

	static const std::string ReadFile( const std::string& path );
	static const void WriteFile( const std::string& path, const std::string& data );
	
};

}
