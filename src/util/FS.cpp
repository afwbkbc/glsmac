#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <unordered_map>

#include "FS.h"

#if defined( DEBUG ) || defined( FASTDEBUG )
#define Log( _text ) std::cout << "<Util::FS> " << (_text) << std::endl
#else
#define Log( _text )
#endif

const std::unordered_map< std::string, std::vector< unsigned char > >& GetEmbeddedFiles();
static const std::unordered_map< std::string, std::vector< unsigned char > >& s_embedded_files = GetEmbeddedFiles();

namespace util {

const char FS::PATH_SEPARATOR =
#ifdef _WIN32
	'\\'
#else
	'/'
#endif
;

const char FS::EXTENSION_SEPARATOR = '.';

const std::string FS::GetExistingCaseSensitivePath( const std::string& base_path, const std::string& case_insensitive_path ) {

#define CHK( _var ) \
    if ( Exists( base_path + PATH_SEPARATOR + _var ) ) { \
        return base_path + PATH_SEPARATOR + _var; \
    }

	// original file
	CHK( case_insensitive_path );

	// all uppercase
	std::string str_upper = case_insensitive_path;
	std::transform( case_insensitive_path.begin(), case_insensitive_path.end(), str_upper.begin(), ::toupper );
	CHK( str_upper );

	// all lowercase
	std::string str_lower = case_insensitive_path;
	std::transform( case_insensitive_path.begin(), case_insensitive_path.end(), str_lower.begin(), ::tolower );
	CHK( str_lower );

	const auto ext = util::FS::GetExtension( case_insensitive_path );
	const auto extpos = case_insensitive_path.length() - ext.length();

	// lowercase with uppercase extension
	std::transform( ext.begin(), ext.end(), str_lower.begin() + extpos, ::toupper );
	CHK( str_lower );

	// uppercase with lowercase extension
	std::transform( ext.begin(), ext.end(), str_upper.begin() + extpos, ::tolower );
	CHK( str_upper );

	// lowercase starting from capital letter (uppercase extension)
	std::transform( case_insensitive_path.begin(), case_insensitive_path.begin() + 1, str_lower.begin(), ::toupper );
	CHK( str_lower );

	// lowercase starting from capital letter (lowercase extension)
	std::transform( ext.begin(), ext.end(), str_lower.begin() + extpos, ::tolower );
	CHK( str_lower );

	// still no luck, fetch entire directory and go file by file lowercase-to-lowercase
	const auto files = ListDirectory( base_path, false );
	std::vector< std::string > lowercase_files = {};
	lowercase_files.reserve( files.size() );
	for ( auto file : files ) {
		std::transform( file.begin(), file.end(), file.begin(), ::tolower );
		lowercase_files.push_back( file );
	}
	ASSERT_NOLOG( files.size() == lowercase_files.size(), "files vector mismatch" );
	std::transform( case_insensitive_path.begin(), case_insensitive_path.begin() + 1, str_lower.begin(), ::tolower );
	for ( size_t i = 0 ; i < lowercase_files.size() ; i++ ) {
		if ( lowercase_files.at( i ) == str_lower ) {
			return base_path + PATH_SEPARATOR + files.at( i );
		}

	}

	// give up
	return "";
}

const std::string FS::NormalizePath( const std::string& path, const char path_separator ) {
	if ( path_separator == PATH_SEPARATOR ) {
		return path;
	}
	else {
		std::string result = path;
		std::replace( result.begin(), result.end(), path_separator, PATH_SEPARATOR );
		return result;
	}
}

const std::string FS::ConvertPath( const std::string& path, const char path_separator ) {
	if ( path_separator == PATH_SEPARATOR ) {
		return path;
	}
	else {
		std::string result = path;
		std::replace( result.begin(), result.end(), PATH_SEPARATOR, path_separator );
		return result;
	}
}

const std::string FS::GetUpDirString() {
	return "..";
}

const std::string FS::GeneratePath( const std::vector< std::string >& parts, const char path_separator ) {
	std::string result = "";
	for ( const auto& part : parts ) {
		if ( !result.empty() ) {
			result += path_separator;
		}
		result += part;
	}
	return result;
}

#ifdef _WIN32

const bool FS::IsWindowsDriveLabel( const std::string& directory ) {
	return
		directory.size() == 2 &&
		directory[0] >= 'A' &&
		directory[0] <= 'Z' &&
		directory[1] == ':' // C:, D:, ...
	;
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

bool FS::IsAbsolutePath( const std::string& path, const char path_separator ) {
	return
#ifdef _WIN32
		(  // the very root (without drive)
			path.size() == 1 &&
			path[0] == path_separator
		) ||
		(
			path.size() >= 2 &&
			path[0] >= 'A' &&
			path[0] <= 'Z' &&
			path[1] == ':' &&
			(
				path.size() == 2 ||
				(
					path.size() >= 3 &&
					path[ 2 ] == path_separator
				)
			)
		)
#else
		!path.empty() &&
			path[ 0 ] == '/'
#endif
		;
}

const std::string FS::GetCurrentDirectory( const char path_separator ) {
	return ConvertPath( std::filesystem::current_path().string(), path_separator );
}

const std::string FS::GetDirName( const std::string& path, const char path_separator ) {
	const auto pos = path.rfind( path_separator );
	if ( pos == std::string::npos ) {
		return "";
	}
	else {
		return path.substr( 0, pos );
	}
}

const std::string FS::GetBaseName( const std::string& path, const char path_separator ) {
	const auto pos = path.rfind( path_separator );
	if ( pos == std::string::npos ) {
		return path;
	}
	else {
		return path.substr( pos + 1 );
	}
}

const std::string FS::GetFilteredPath( const std::string& path, const char path_separator ) {
	size_t pos = 0;
	if ( path == "." ) {
		return "";
	}
	while ( pos + 1 < path.size() && path[ pos ] == EXTENSION_SEPARATOR && path[ pos + 1 ] == path_separator ) {
		pos += 2;
	}
	return pos
		? path.substr( pos )
		: path;
}

const std::string FS::GetAbsolutePath( const std::string& path, const char path_separator ) {
	return IsAbsolutePath( path )
		? path
		: GetCurrentDirectory() + path_separator + GetFilteredPath( path );
}

const std::string FS::GetExtension( const std::string& path, const char path_separator ) {
	const auto pos = path.rfind( EXTENSION_SEPARATOR );
	if ( pos == std::string::npos ) {
		return "";
	}
	else {
		const auto pathsep_pos = path.rfind( path_separator );
		if ( pathsep_pos != std::string::npos && pathsep_pos > pos ) {
			return "";
		}
		return path.substr( pos );
	}
}

const std::vector< std::string > FS::GetExtensions( const std::string& path, const char path_separator ) {
	std::vector< std::string > result = {};
	size_t pos, path_pos, last_pos = path.size();
	while ( ( pos = path.rfind( EXTENSION_SEPARATOR, last_pos - 1 ) ) != std::string::npos ) {
		path_pos = path.rfind( path_separator, last_pos - 1 );
		if ( path_pos != std::string::npos && path_pos > pos ) {
			break;
		}
		result.push_back( path.substr( pos, last_pos - pos ) );
		last_pos = pos;
	}
	std::reverse( result.begin(), result.end() );
	return result;
}

const bool FS::Exists( const std::string& path, const char path_separator ) {
	return std::filesystem::exists( NormalizePath( path, path_separator ) );
}

const bool FS::IsFile( const std::string& path, const char path_separator ) {
	return std::filesystem::is_regular_file( NormalizePath( path, path_separator ) );
}

const bool FS::FileExists( const std::string& path, const char path_separator ) {
	return Exists( path, path_separator ) && IsFile( path, path_separator );
}

const bool FS::IsDirectory( const std::string& path, const char path_separator ) {
	return std::filesystem::is_directory( NormalizePath( path, path_separator ) );
}

const bool FS::DirectoryExists( const std::string& path, const char path_separator ) {
	return Exists( path, path_separator ) && IsDirectory( path, path_separator );
}

void FS::CreateDirectoryIfNotExists( const std::string& path, const char path_separator ) {
	if ( !DirectoryExists( path, path_separator ) ) {
		//Log( "Creating directory: " + path );
		std::filesystem::create_directories( NormalizePath( path, path_separator ) );
	}
}

#ifdef _WIN32
std::vector< std::string > FS::ListDirectory( std::string directory, const bool return_absolute_paths, const char path_separator ) {
	ASSERT_NOLOG( !directory.empty(), "directory is empty" );
#else

std::vector< std::string > FS::ListDirectory( const std::string& directory, const bool return_absolute_paths, const char path_separator ) {
#endif
	std::vector< std::string > result = {};

	//Log( "Reading directory: " + directory );

	try {

#ifdef _WIN32
		const bool is_windows_drive_label = IsWindowsDriveLabel( directory );
		if ( is_windows_drive_label ) {
			directory += path_separator;
		}
#endif

		std::vector< std::filesystem::path > items;
		std::copy(
			std::filesystem::directory_iterator(
				!directory.empty()
					? directory
					: std::string( 1, path_separator )
			),
			std::filesystem::directory_iterator(),
			std::back_inserter( items )
		);
		std::sort( items.begin(), items.end() );

		for ( const auto& item : items ) {
			const auto item_str = item.string();
			const uint8_t prefix_len = ( directory.empty() || directory[ directory.size() - 1 ] == path_separator )
				? 0
				: 1;
			ASSERT_NOLOG( item_str.substr( 0, directory.size() ) == directory, "unexpected path in directory list results: " + item_str );
			result.push_back(
				ConvertPath(
					return_absolute_paths
						? item_str
						: item_str.substr( directory.size() + prefix_len ), path_separator
				)
			);
		}
	}
	catch ( std::filesystem::filesystem_error& e ) {
		// permission denied etc
		// TODO: display error?
		return result;
	}

	return result;
}

void FS::ReadFile( std::vector< unsigned char >& buffer, const std::string& path, const char path_separator ) {
	//Log( "Reading file: " + path );
	ASSERT_NOLOG( FileExists( path, path_separator ), "file \"" + path + "\" does not exist or is not a file" );
	std::ifstream in( NormalizePath( path, path_separator ), std::ios_base::binary );
	in.seekg( 0, std::ios::end );
	const auto sz = in.tellg();
	in.seekg( 0, std::ios::beg );
	buffer.resize( sz );
	in.read( (char*)&buffer[ 0 ], sz );
	in.close();
}

const std::string FS::ReadTextFile( const std::string& path, const char path_separator ) {
	//Log( "Reading file: " + path );
	ASSERT_NOLOG( FileExists( path, path_separator ), "file \"" + path + "\" does not exist or is not a file" );
	std::stringstream data;
	std::ifstream in( NormalizePath( path, path_separator ), std::ios_base::binary );
	while ( data << in.rdbuf() ) {}
	in.close();
	return data.str();
}

const void FS::WriteFile( const std::string& path, const std::string& data, const char path_separator ) {
	//Log( "Writing file: " + path );
	std::ofstream out( NormalizePath( path, path_separator ), std::ios_base::binary );
	out << data;
	out.close();
}

const std::vector< unsigned char >& FS::GetEmbeddedFile( const std::string& key ) {
	//Log( "Reading embedded file: " + key );
	ASSERT_NOLOG( s_embedded_files.find( key ) != s_embedded_files.end(), "embedded file \"" + key + "\" does not exist" );
	return s_embedded_files.at( key );
}

}
