#include "WindowsInstallPaths.h"

#ifdef _WIN32

#include <cstdlib>
#include <filesystem>
#include <unordered_set>
#include <vector>

#include "util/FS.h"
#include "util/String.h"

namespace config {

namespace {

const std::string GetRegistryString(
	HKEY root,
	const std::string& key_path,
	const std::string& value_name,
	const REGSAM registry_view
) {
	HKEY key = nullptr;
	if ( RegOpenKeyExA( root, key_path.c_str(), 0, KEY_READ | registry_view, &key ) != ERROR_SUCCESS ) {
		return "";
	}

	DWORD type = 0;
	DWORD size = 0;
	const auto query_result = RegQueryValueExA( key, value_name.c_str(), nullptr, &type, nullptr, &size );
	if (
		query_result != ERROR_SUCCESS ||
		( type != REG_SZ && type != REG_EXPAND_SZ ) ||
		size == 0
	) {
		RegCloseKey( key );
		return "";
	}

	std::vector< char > value( size + 1, '\0' );
	const auto read_result = RegQueryValueExA(
		key,
		value_name.c_str(),
		nullptr,
		&type,
		reinterpret_cast< LPBYTE >( value.data() ),
		&size
	);
	RegCloseKey( key );
	if ( read_result != ERROR_SUCCESS ) {
		return "";
	}

	std::string result = value.data();
	if ( type == REG_EXPAND_SZ ) {
		const auto expanded_size = ExpandEnvironmentStringsA( result.c_str(), nullptr, 0 );
		if ( expanded_size > 0 ) {
			std::vector< char > expanded( expanded_size, '\0' );
			if ( ExpandEnvironmentStringsA( result.c_str(), expanded.data(), expanded_size ) > 0 ) {
				result = expanded.data();
			}
		}
	}
	return result;
}

const std::string GetEnvironmentPath( const std::string& name ) {
	const auto* const value = std::getenv( name.c_str() );
	return value == nullptr
		? ""
		: value;
}

void AddExistingPath(
	std::vector< std::string >& result,
	std::unordered_set< std::string >& seen,
	const std::string& path
) {
	if ( path.empty() ) {
		return;
	}

	try {
		if ( !util::FS::DirectoryExists( path ) ) {
			return;
		}
		const auto normalized = util::FS::NormalizePath( path );
		auto key = normalized;
		util::String::ToLowerCase( key );
		if ( seen.insert( key ).second ) {
			result.push_back( normalized );
		}
	}
	catch ( const std::filesystem::filesystem_error& ) {
		// Ignore stale or malformed installer records and continue searching.
	}
}

void AddStorePaths(
	std::vector< std::string >& result,
	std::unordered_set< std::string >& seen,
	const std::string& store_root,
	const std::vector< std::string >& subdirectories
) {
	if ( store_root.empty() ) {
		return;
	}
	for ( const auto& subdirectory : subdirectories ) {
		AddExistingPath(
			result,
			seen,
			util::FS::GeneratePath(
				{
					store_root,
					subdirectory,
				}
			)
		);
	}
}

}

const std::vector< std::string > GetWindowsSMACInstallPaths() {
	std::vector< std::string > result = {};
	std::unordered_set< std::string > seen = {};

	struct registry_location_t {
		HKEY root;
		std::string key;
		std::string value;
	};
	// GOG product 1207658936 and Steam app 2204130 use these installer records.
	const std::vector< registry_location_t > install_locations = {
		{
			HKEY_LOCAL_MACHINE,
			"SOFTWARE\\GOG.com\\Games\\1207658936",
			"path",
		},
		{
			HKEY_CURRENT_USER,
			"SOFTWARE\\GOG.com\\Games\\1207658936",
			"path",
		},
		{
			HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\1207658936_is1",
			"InstallLocation",
		},
		{
			HKEY_CURRENT_USER,
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\1207658936_is1",
			"InstallLocation",
		},
		{
			HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 2204130",
			"InstallLocation",
		},
		{
			HKEY_CURRENT_USER,
			"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Steam App 2204130",
			"InstallLocation",
		},
	};
	const std::vector< REGSAM > registry_views = {
		KEY_WOW64_32KEY,
		KEY_WOW64_64KEY,
	};
	for ( const auto registry_view : registry_views ) {
		for ( const auto& location : install_locations ) {
			AddExistingPath(
				result,
				seen,
				GetRegistryString( location.root, location.key, location.value, registry_view )
			);
		}
	}

	const std::vector< std::string > product_directories = {
		"Sid Meier's Alpha Centauri",
		"Sid Meier's Alpha Centauri Planetary Pack",
	};
	const std::vector< std::string > steam_subdirectories = {
		util::FS::GeneratePath( { "steamapps", "common", product_directories[ 0 ] } ),
		util::FS::GeneratePath( { "steamapps", "common", product_directories[ 1 ] } ),
	};
	const std::vector< registry_location_t > steam_locations = {
		{
			HKEY_CURRENT_USER,
			"SOFTWARE\\Valve\\Steam",
			"SteamPath",
		},
		{
			HKEY_LOCAL_MACHINE,
			"SOFTWARE\\Valve\\Steam",
			"InstallPath",
		},
	};
	for ( const auto registry_view : registry_views ) {
		for ( const auto& location : steam_locations ) {
			AddStorePaths(
				result,
				seen,
				GetRegistryString( location.root, location.key, location.value, registry_view ),
				steam_subdirectories
			);
		}
	}

	const std::vector< std::string > program_files_roots = {
		GetEnvironmentPath( "ProgramFiles" ),
		GetEnvironmentPath( "ProgramFiles(x86)" ),
	};
	for ( const auto& program_files : program_files_roots ) {
		if ( program_files.empty() ) {
			continue;
		}
		AddStorePaths(
			result,
			seen,
			util::FS::GeneratePath( { program_files, "Steam" } ),
			steam_subdirectories
		);
		for ( const auto& product_directory : product_directories ) {
			AddExistingPath(
				result,
				seen,
				util::FS::GeneratePath(
					{
						program_files,
						"GOG Galaxy",
						"Games",
						product_directory,
					}
				)
			);
		}
	}

	const auto system_drive = GetEnvironmentPath( "SystemDrive" );
	if ( !system_drive.empty() ) {
		for ( const auto& product_directory : product_directories ) {
			AddExistingPath(
				result,
				seen,
				util::FS::GeneratePath(
					{
						system_drive,
						"GOG Games",
						product_directory,
					}
				)
			);
		}
	}

	return result;
}

}

#endif
