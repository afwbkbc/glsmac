#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

#include "FS.h"

using namespace std;

#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

namespace util {

	const std::string FS::GetPathSeparator() {
		return PATH_SEPARATOR;
	}

	const std::string FS::GetUpDirString() {
		return "..";
	}

#ifdef _WIN32

	const bool FS::IsWindowsDriveLabel( const std::string& directory ) {
		return directory.size() == 2 && directory[1] == ':'; // C:, D:, ...
		// todo: multiletter drives?
	}
	
	const std::vector< std::string > FS::GetWindowsDrives() {
		std::vector< std::string > result = {};
		const auto drives = GetLogicalDrives();
		ASSERT_NOLOG( sizeof( drives ) == 4, "drives bitmask is not 32-bit" );
		for ( uint8_t i = 0; i < ( 'Z' - 'A' ); i++ ) {
			if ( drives & ( 1 << i ) ) {
				result.push_back( std::string(1, 'A' + i) + ":" );
			}
		}
		return result;
	}

#endif
	
	const std::string FS::GetCurrentDirectory() {
		return std::filesystem::current_path().string();
	}

	const std::string FS::GetDirName( const std::string& path ) {
		const auto pos = path.rfind( PATH_SEPARATOR );
		if ( pos == std::string::npos ) {
			return "";
		}
		else {
			return path.substr( 0, pos );
		}
	}
	
	const std::string FS::GetBaseName( const std::string& path ) {
		const auto pos = path.rfind( PATH_SEPARATOR );
		if ( pos == std::string::npos ) {
			return path;
		}
		else {
			return path.substr( pos + 1 );
		}
	}
	
	const bool FS::Exists( const string& path ) {
		return std::filesystem::exists( path );
	}

	const bool FS::IsFile( const string& path ) {
		return std::filesystem::is_regular_file( path );
	}

	const bool FS::FileExists( const string& path ) {
		return Exists( path ) && IsFile( path );
	}

	const bool FS::IsDirectory( const string& path ) {
		return std::filesystem::is_directory( path );
	}

	const bool FS::DirectoryExists( const string& path ) {
		return Exists( path ) && IsDirectory( path );
	}

	void FS::CreateDirectoryIfNotExists( const string& path ) {
		if ( !DirectoryExists( path ) ) {
			std::filesystem::create_directory( path );
		}
	}

#ifdef _WIN32
	std::vector< std::string > FS::ListDirectory( std::string directory, const bool return_absolute_paths ) {
		ASSERT_NOLOG( !directory.empty(), "directory is empty" );
#else
	std::vector< std::string > FS::ListDirectory( const std::string& directory, const bool return_absolute_paths ) {
#endif
		std::vector< std::string > result = {};

		try {

#ifdef _WIN32
			ASSERT_NOLOG( directory.find(':') != std::string::npos, "invalid windows path '" + directory + "' (must contain a colon)" );
			const bool is_windows_drive_label = IsWindowsDriveLabel( directory );
			if ( is_windows_drive_label ) {
				directory += PATH_SEPARATOR;
			}
#endif
			std::vector< std::filesystem::path > items;
			std::copy(
				std::filesystem::directory_iterator(
					!directory.empty()
						? directory
						: PATH_SEPARATOR
				),
				std::filesystem::directory_iterator(),
				std::back_inserter( items )
			);
			std::sort( items.begin(), items.end() );

			for (const auto& item : items) {
				const auto item_str = item.string();
				const uint8_t prefix_len = directory[ directory.size() - 1 ] == PATH_SEPARATOR[ 0 ] ? 0 : 1;
				ASSERT_NOLOG( item_str.substr( 0, directory.size() ) == directory, "unexpected path in directory list results: " + item_str );
				result.push_back( return_absolute_paths ? item_str : item_str.substr( directory.size() + prefix_len) );
			}
		}
		catch ( std::filesystem::filesystem_error& e ) {
			// permission denied etc
			// TODO: display error?
			return result;
		}

		return result;
	}

	const string FS::ReadFile( const string& path ) {
		ASSERT_NOLOG( FileExists( path ), "file \"" + path + "\" does not exist or is not a file" );
		stringstream data;
		ifstream in( path, std::ios_base::binary );
		while ( data << in.rdbuf() );
		in.close();
		return data.str();
	}

	const void FS::WriteFile( const string& path, const string& data ) {
		ofstream out( path, std::ios_base::binary );
		out << data;
		out.close();
	}

}

