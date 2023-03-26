#pragma once

#include <vector>
#include <string>

#include "Util.h"

namespace util {
	
CLASS( FS, Util )
	
	static const std::string GetPathSeparator();

	static const std::string GetCurrentDirectory();
	
	static const bool Exists( const std::string& path );
	static const bool IsFile( const std::string& path );
	static const bool FileExists( const std::string& path );
	static const bool IsDirectory( const std::string& path );
	static const bool DirectoryExists( const std::string& path );

	static void CreateDirectoryIfNotExists( const std::string& path );

	static std::vector< std::string > ListDirectory( const std::string& directory, const bool return_absolute_paths = false );
	
	static const std::string ReadFile( const std::string& path );
	static const void WriteFile( const std::string& path, const std::string& data );
	
};

}
