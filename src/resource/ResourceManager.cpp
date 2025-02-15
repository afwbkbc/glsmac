#include "ResourceManager.h"

#include <unordered_map>
#include <algorithm>

#include "util/FS.h"

#include "engine/Engine.h"
#include "config/Config.h"

namespace resource {

ResourceManager::ResourceManager( const std::string& data_path )
	: m_resources_to_filenames(
	{

		{
			PCX_PALETTE,
			"palette.pcx"
		},
		{
			PCX_INTERFACE,
			"interface.pcx"
		},
		{
			PCX_JACKAL,
			"jackal.pcx"
		},
		{
			PCX_ICONS,
			"icons.pcx"
		},
		{
			PCX_NEWICONS,
			"newicons.pcx"
		},
		{
			PCX_ALIENCIT,
			"aliencit.pcx"
		},
		{
			PCX_FLAGS,
			"flags.pcx"
		},
		{
			PCX_UNITS,
			"units.pcx"
		},
		{
			PCX_TEXTURE,
			"texture.pcx"
		},
		{
			PCX_TER1,
			"ter1.pcx"
		},
		{
			PCX_CONSOLE_X,
			"console_x.pcx"
		},
		{
			PCX_CONSOLE_X2_A,
			"console_x2_a.pcx"
		},
		{
			PCX_CONSOLE2_A,
			"console2_a.pcx"
		},
		{
			PCX_LOGO,
			"logo.pcx"
		},
		{
			PCX_OPENINGA,
			"openinga.pcx"
		},
		{
			PCX_SPACE_SM,
			"space_sm.pcx"
		},
#define xM( _m ) { PCX_MOON##_m, "moon"#_m".pcx" }
		xM( 1 ),
		xM( 2 ),
		xM( 3 ),
#undef xM
		{
			PCX_MOON1,
			"moon1.pcx"
		},
		{
			PCX_MOON2,
			"moon2.pcx"
		},
		{
			PCX_MOON3,
			"moon3.pcx"
		},
#define xSLC( _s, _l, _c ) { PCX_S##_s##L##_l##C##_c, "S"#_s"L"#_l"C"#_c".pcx" }
#define xSL( _s, _l ) \
    xSLC( _s, _l, 1 ), \
    xSLC( _s, _l, 2 ), \
    xSLC( _s, _l, 3 )
#define xS( _s ) \
    xSL( _s, 1 ), \
    xSL( _s, 2 ), \
    xSL( _s, 3 )
		xS( 1 ),
		xS( 2 ),
		xS( 3 ),
#undef xS
#undef xSL
#undef xSLC
		{
			PCX_XI,
			"xi.pcx"
		},
		{
			PCX_XF,
			"xf.pcx"
		},
		{
			PCX_GAIANS,
			"gaians.pcx"
		},
		{
			PCX_HIVE,
			"hive.pcx"
		},
		{
			PCX_UNIV,
			"univ.pcx"
		},
		{
			PCX_MORGAN,
			"morgan.pcx"
		},
		{
			PCX_SPARTANS,
			"spartans.pcx"
		},
		{
			PCX_BELIEVE,
			"believe.pcx"
		},
		{
			PCX_PEACE,
			"peace.pcx"
		},
		{
			PCX_CYBORG,
			"cyborg.pcx"
		},
		{
			PCX_PIRATES,
			"pirates.pcx"
		},
		{
			PCX_DRONE,
			"drone.pcx"
		},
		{
			PCX_ANGELS,
			"angels.pcx"
		},
		{
			PCX_FUNGBOY,
			"fungboy.pcx"
		},
		{
			PCX_CARETAKE,
			"caretake.pcx"
		},
		{
			PCX_USURPER,
			"usurper.pcx"
		},

		{
			TTF_ARIALN,
			"arialn.ttf"
		},
		{
			TTF_ARIALNB,
			"arialnb.ttf"
		},

		{
			WAV_OPENING_MENU,
			"opening menu.wav"
		},
		{
			WAV_OK,
			"ok.wav"
		},
		{
			WAV_MENU_UP,
			"menu up.wav"
		},
		{
			WAV_MENU_DOWN,
			"menu down.wav"
		},
		{
			WAV_MENU_OUT,
			"menu out.wav"
		},
		{
			WAV_AMENU2,
			"amenu2.wav"
		},
		{
			WAV_MMENU,
			"mmenu.wav"
		},
		{
			WAV_TURN_COMPLETE,
			"cpu turn complete.wav"
		},
		{
			WAV_PLS_DONT_GO,
			"CPU please don't go.wav"
		},

	}
) {

	m_data_path = util::FS::GeneratePath(
		{
			data_path,
			"default"
		}
	);

	for ( const auto& it : m_resources_to_filenames ) {
		m_filenames_to_resources.insert(
			{
				it.second,
				it.first
			}
		);
	}
}

void ResourceManager::Init( const std::vector< std::string >& possible_smac_paths, const config::smac_type_t smac_type ) {
	const bool print_errors = smac_type != config::ST_AUTO;
	for ( const auto& path : possible_smac_paths ) {

		// GOG / Steam
		if (
			( smac_type == config::ST_GOG || smac_type == config::ST_STEAM || smac_type == config::ST_AUTO ) &&
				CheckFiles(
					path, {
						"terran.exe",
						"terranx.exe",
						"arialnb.ttf",
						"arialnbi.ttf",
						"arialni.ttf",
					}, print_errors
				) && ResolveBuiltins(
				path, {
					{
						".wav",
						"fx"
					},
				}, PM_NONE, {}, print_errors
			)
			) {
			m_detected_smac_type = smac_type == config::ST_AUTO
				? config::ST_STEAM // for now there don't seem to be any differences between Steam and GoG so can pick whatever
				: smac_type;
			return; // found GOG or Steam
		}

		// Loki
		if (
			( smac_type == config::ST_LOKI || smac_type == config::ST_AUTO ) &&
				CheckFiles(
					path, {
						"smac",
						"smac.sh",
						"smacx",
						"smacx.sh",
					}, print_errors
				) && ResolveBuiltins(
				path + util::FS::PATH_SEPARATOR + "data", {
					{
						".wav",
						"fx"
					},
					{
						".ttf",
						"fonts"
					}
				}, PM_SPACES_TO_UNDERSCORES, {}, print_errors
			)
			) {
			m_detected_smac_type = config::ST_LOKI;
			return;
		}

		// Legacy
		if (
			( smac_type == config::ST_LEGACY || smac_type == config::ST_AUTO ) &&
				CheckFiles(
					path, {
						"terran.exe",
						"terranx.exe",
						"arialb.ttf", // legacy has different fonts
						"ariali.ttf", // legacy has different fonts
						"arialr.ttf", // legacy has different fonts
					}, print_errors
				) && ResolveBuiltins(
				path, {
					{
						".wav",
						"fx"
					},
				}, PM_NONE, {
					{
						TTF_ARIALNB,
						"arialn.ttf"
					},
					{
						PCX_LOGO,
						"openinga.pcx"
					},
				}, print_errors
			)
			) {
			m_detected_smac_type = config::ST_LEGACY;
			return;
		}

		if ( smac_type != config::ST_AUTO ) {
			break; // don't search in . if specific type was given
		}
	}
	std::string paths = "";
	for ( const auto& path : possible_smac_paths ) {
		paths += path + "; ";
	}
	const std::string msg = "Unable to find SMAC distribution (tried paths: " + paths + "). Run from SMAC directory or pass it with --smacpath argument";
	Log( msg );
	if ( smac_type == config::ST_AUTO ) {
		Log( "Also try --smactype with your SMAC installation type (e.g. GoG, Loki, Planetary Pack), this will give more descriptive errors if some files are not found." );
	}
	THROW( msg );
}

const config::smac_type_t ResourceManager::GetDetectedSMACType() const {
	return m_detected_smac_type;
}

const resource_t ResourceManager::GetResource( const std::string& filename ) const {
	const auto it = m_filenames_to_resources.find( filename );
	if ( it != m_filenames_to_resources.end() ) {
		return it->second;
	}
	return NONE;
}

const std::string& ResourceManager::GetFilename( const resource_t res ) const {
	ASSERT_NOLOG( m_resources_to_filenames.find( res ) != m_resources_to_filenames.end(), "filename for " + std::to_string( res ) + " not found" );
	return m_resources_to_filenames.at( res );
}

const std::string& ResourceManager::GetPath( const resource_t res ) const {
	ASSERT_NOLOG( m_resource_paths.find( res ) != m_resource_paths.end(), "resource path for " + std::to_string( res ) + " not found" );
	return m_resource_paths.at( res );
}

const std::string& ResourceManager::TryGetCustomPath( const std::string& path ) {
	std::string key = "";
	key.resize( path.length() );
	std::transform( path.begin(), path.end(), key.begin(), ::tolower );

	std::string resolved_file = "";

	// look in mod dirs
	for ( const auto& mod_path : g_engine->GetConfig()->GetModPaths() ) {
		resolved_file = util::FS::GetExistingCaseSensitivePath( mod_path, path );
	}

	if ( resolved_file.empty() ) {
		// look in datadir
		resolved_file = util::FS::GetExistingCaseSensitivePath( m_data_path, path );
	}

	if ( resolved_file.empty() ) {

		// look in builtin paths
		const auto& it = m_custom_resource_paths.find( key );
		if ( it != m_custom_resource_paths.end() ) {
			return it->second;
		}

		// look in SMAC dir
		resolved_file = util::FS::GetExistingCaseSensitivePath( m_smac_path, GetFixedPath( path, m_extension_path_map, m_path_modifiers ) );
	}
	if ( !resolved_file.empty() && !util::FS::IsFile( resolved_file ) ) {
		resolved_file = "";
	}
	if ( !resolved_file.empty() ) {
		Log( "Resolved resource \"" + path + "\" to " + resolved_file );
	}
	else {
		Log( "Failed to resolve resource \"" + path + "\"" );
	}
	return m_custom_resource_paths.insert(
		{
			key,
			resolved_file
		}
	).first->second;
}

const std::string& ResourceManager::GetCustomPath( const std::string& path ) {
	const auto& result = TryGetCustomPath( path );
	if ( result.empty() ) {
		THROW( "could not resolve resource (path does not exist or is not a file: " + path + ")" );
	}
	return result;
}

const std::string ResourceManager::GetFixedPath( const std::string& file, const extension_path_map_t& extension_path_map, const path_modifier_t path_modifiers ) {
	std::string fixed_path = "";
	const auto& it = extension_path_map.find( util::FS::GetExtension( file ) );
	if ( it != extension_path_map.end() ) {
		fixed_path += it->second + util::FS::PATH_SEPARATOR;
	}
	fixed_path += file;
	if ( path_modifiers & PM_SPACES_TO_UNDERSCORES ) {
		std::replace( fixed_path.begin(), fixed_path.end(), ' ', '_' );
	}
	return fixed_path;
}

const bool ResourceManager::CheckFiles( const std::string& path, const std::vector< std::string >& files, const bool print_errors ) const {
	for ( const auto& file : files ) {
		const auto resolved_file = util::FS::GetExistingCaseSensitivePath( path, file );
		if ( resolved_file.empty() || !util::FS::IsFile( resolved_file ) ) {
			if ( print_errors ) {
				Log(
					"Could not find file: " + util::FS::GeneratePath(
						{
							path,
							file
						}
					)
				);
			}
			return false;
		}
	}
	return true;
}

const bool ResourceManager::ResolveBuiltins( const std::string& path, const extension_path_map_t& extension_path_map, const path_modifier_t path_modifiers, const resource_substitutes_t& substitutes, const bool print_errors ) {
	std::unordered_map< resource::resource_t, std::string > resolved_files = {};
	resolved_files.reserve( m_resources_to_filenames.size() );
	for ( const auto& it : m_resources_to_filenames ) {
		std::string file = it.second;
		const auto& sub_it = substitutes.find( it.first );
		if ( sub_it != substitutes.end() ) {
			file = sub_it->second;
		}
		const auto resolved_file = util::FS::GetExistingCaseSensitivePath( path, GetFixedPath( file, extension_path_map, path_modifiers ) );
		if ( resolved_file.empty() || !util::FS::IsFile( resolved_file ) ) {
			if ( print_errors ) {
				Log(
					"Could not resolve file: " + util::FS::GeneratePath(
						{
							path,
							file
						}
					)
				);
			}
			return false;
		}
		resolved_files.insert(
			{
				it.first,
				resolved_file
			}
		);
	}
	ASSERT_NOLOG( resolved_files.size() == m_resources_to_filenames.size(), "some files were not resolved" );
	ASSERT_NOLOG( m_resource_paths.empty(), "resource paths not empty" );
	m_smac_path = path;
	m_resource_paths = resolved_files;
	m_extension_path_map = extension_path_map;
	m_path_modifiers = path_modifiers;
	for ( const auto& it : resolved_files ) {
		Log( "Resolved resource #" + std::to_string( it.first ) + " to " + it.second );
	}
	return true;
}

}
