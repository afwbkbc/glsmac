#pragma once

#include <vector>
#include <string>

#include "Util.h"

namespace util {

CLASS( FS, Util )

	static const char PATH_SEPARATOR;
	static const char EXTENSION_SEPARATOR;

	// to workaround uppercase or lowercase filenames
	static const std::string GetExistingCaseSensitivePath( const std::string& base_path, const std::string& case_insensitive_path );

	static const std::string NormalizePath( const std::string& path, const char path_separator );
	static const std::string ConvertPath( const std::string& path, const char path_separator );

	static const std::string GetUpDirString();
	static const std::string GeneratePath( const std::vector< std::string >& parts, const char path_separator = PATH_SEPARATOR );

#ifdef _WIN32
	static const bool IsWindowsDriveLabel( const std::string& directory );
	static const std::vector< std::string> GetWindowsDrives();
#endif

	static bool IsAbsolutePath( const std::string& path, const char path_separator = PATH_SEPARATOR );

	static const std::string GetCurrentDirectory( const char path_separator = PATH_SEPARATOR );
	static const std::string GetDirName( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const std::string GetBaseName( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const std::string GetFilteredPath( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const std::string GetAbsolutePath( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const std::string GetExtension( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const std::vector< std::string > GetExtensions( const std::string& path, const char path_separator = PATH_SEPARATOR );

	static const bool Exists( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const bool IsFile( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const bool FileExists( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const bool IsDirectory( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const bool DirectoryExists( const std::string& path, const char path_separator = PATH_SEPARATOR );

	static void CreateDirectoryIfNotExists( const std::string& path, const char path_separator = PATH_SEPARATOR );

#ifdef _WIN32
	static std::vector< std::string > ListDirectory( std::string directory, const bool return_absolute_paths, const char path_separator = PATH_SEPARATOR );
#else

	static std::vector< std::string > ListDirectory( const std::string& directory, const bool return_absolute_paths, const char path_separator = PATH_SEPARATOR );

#endif

	static void ReadFile( std::vector< unsigned char >& buffer, const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const std::string ReadTextFile( const std::string& path, const char path_separator = PATH_SEPARATOR );
	static const void WriteFile( const std::string& path, const std::string& data, const char path_separator = PATH_SEPARATOR );

	static const std::vector< unsigned char >& GetEmbeddedFile( const std::string& key );

};

}
