#include <stdexcept>
#include <sstream>

#include "Config.h"

#include "version.h"

#include "util/ConfigManager.h"
#include "util/FS.h"
#include "util/String.h"
#include "util/random/Random.h"
#include "util/LogHelper.h"

namespace config {

WRAPIMPL_BEGIN( Config )
	WRAPIMPL_PROPS};
	for ( const auto& it : m_manager->GetSettings() ) {
		properties.insert( { it.first, VALUE( gse::value::String, , it.second.second ) } );
	}
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Config )

void Config::Error( const std::string& error ) {
	util::LogHelper::Println(
		(std::string)
			"\n" +
			+"ERROR: " + error + "\n" +
			+"\n"
			+ m_manager->GetUnknownArgumentNote() + "\n"
	);
	exit( EXIT_FAILURE );
};

const types::Vec2< size_t > Config::ParseSize( const std::string& value ) {
	const size_t pos = value.find( 'x' );
	const std::string s_invalid_format = "Invalid size specified! Format is WIDTHxHEIGHT, for example: 80x40";
	if ( pos == std::string::npos ) {
		Error( s_invalid_format );
	}
	types::Vec2< size_t > result = {};
	try {
		result.x = std::stoul( value.substr( 0, pos ) );
		result.y = std::stoul( value.substr( pos + 1 ) );
	}
	catch ( std::invalid_argument& e ) {
		Error( s_invalid_format );
	}
	return result;
};

Config::Config( const std::string& path )
	: m_smac_path( "" )
	, m_data_path( "GLSMAC_data" )
	, m_prefix( DEFAULT_GLSMAC_PREFIX + util::FS::PATH_SEPARATOR ) {

	m_manager = new util::ConfigManager( path, GetPrefix() + "config.yml" );

	m_manager->AddRule(
		"benchmark", "Disable VSync and FPS limit", AH( this ) {
			m_launch_flags |= LF_BENCHMARK;
		}
	);
	m_manager->AddRule(
		"datapath", "DATA_PATH", "Specify path to GLSMAC_data directory (default: " + util::FS::GetCurrentDirectory() + "/" + m_data_path + ")", AH( this ) {
			m_data_path = value;
		}
	);
	m_manager->AddRule(
		"help", "Show this message", AH( this ) {
			util::LogHelper::Println( m_manager->GetHelpString() );
			exit( EXIT_SUCCESS );
		}
	);
	m_manager->AddRule(
		"nosound", "Start without sound", AH( this ) {
			m_launch_flags |= LF_NOSOUND;
		}
	);
	m_manager->AddRule(
		"prefix", "GLSMAC_PREFIX", "Path to store GLSMAC data in (default: " + DEFAULT_GLSMAC_PREFIX + ")", AH( this ) {
			m_prefix = value + util::FS::PATH_SEPARATOR;
		}
	);
	m_manager->AddRule(
		"skipintro", "Skip intro", AH( this ) {
			m_launch_flags |= LF_SKIPINTRO;
		}
	);
	m_manager->AddRule(
		"showfps", "Show FPS counter at top left corner", AH( this ) {
			m_launch_flags |= LF_SHOWFPS;
		}
	);
	m_manager->AddRule(
		"smacpath", "SMAC_PATH", "Specify path to SMAC directory (must contain SMACX too)", AH( this ) {
			m_smac_path = value;
		}
	);
	m_manager->AddRule(
		"smactype", "SMAC_TYPE", "Specify type of SMAC installation: gog, loki, steam, legacy (default: autodetect)", AH( this ) {
			const std::unordered_map< std::string, smac_type_t > values = {
				{ "gog",    ST_GOG },
				{ "loki",   ST_LOKI },
				{ "steam",  ST_STEAM },
				{ "legacy", ST_LEGACY },
			};
			const auto& it = values.find( value );
			if ( it != values.end() ) {
				m_smac_type = it->second;
			}
			else {
				std::string errmsg = "Invalid --smactype value specified! Possible choices:";
				for ( const auto& it : values ) {
					errmsg += " " + it.first;
				}
				Error( errmsg );
			}
		}
	);
	m_manager->AddRule(
		"version", "Show version of GLSMAC", AH() {
			util::LogHelper::Println(
				(std::string)
					"\n"
					+ GLSMAC_VERSION_FULL + ' ' + GLSMAC_URL + "\n"
					+ "\n"
					+ GLSMAC_LICENSE + "\n"
			);
			exit( EXIT_SUCCESS );
		}
	);
	m_manager->AddRule(
		"windowed", "Start in windowed mode", AH( this ) {
			m_launch_flags |= LF_WINDOWED;
		}
	);
	m_manager->AddRule(
		"window-size", "WINDOW_SIZE", "Size of window", AH( this ) {
			if ( !HasLaunchFlag( LF_WINDOWED ) ) {
				Error( "Window-related options can only be used after --windowed argument!" );
			}
			m_window_size = ParseSize( value );
			m_launch_flags |= LF_WINDOW_SIZE;
		}
	);

	m_manager->AddRule(
		"quickstart", "Skip intro and main menu and generate/load map directly", AH( this ) {
			m_launch_flags |= LF_QUICKSTART;
		}
	);
	const std::string s_quickstart_argument_missing = "Quickstart-related options can only be used after --quickstart argument!";
	m_manager->AddRule(
		"quickstart-seed", "SEED", "Generate map with specific seed (A:B:C:D)", AH( this, s_quickstart_argument_missing ) {
			if ( !HasLaunchFlag( LF_QUICKSTART ) ) {
				Error( s_quickstart_argument_missing );
			}
			try {
				m_quickstart_seed = util::random::Random::GetStateFromString( value );
			}
			catch ( std::runtime_error& e ) {
				Error( "Invalid seed format! Seed must contain four numbers separated by colon, for example: 1651011033:1377505029:3019448108:3247278135" );
			}
			m_launch_flags |= LF_QUICKSTART_SEED;
		}
	);
	m_manager->AddRule(
		"quickstart-mapfile", "MAP_FILE", "Load from existing map file (*.gsm)", AH( this, s_quickstart_argument_missing ) {
			if ( !HasLaunchFlag( LF_QUICKSTART ) ) {
				Error( s_quickstart_argument_missing );
			}
			if ( !util::FS::FileExists( value ) ) {
				Error( "Map file \"" + value + "\" not found!" );
			}
			m_quickstart_mapfile = value;
			m_launch_flags |= LF_QUICKSTART_MAP_FILE;
		}
	);
	m_manager->AddRule(
		"quickstart-mapsize", "MAP_SIZE", "Generate map of specific size (WxH)", AH( this, s_quickstart_argument_missing ) {
			if ( !HasLaunchFlag( LF_QUICKSTART ) ) {
				Error( s_quickstart_argument_missing );
			}
			m_quickstart_mapsize = ParseSize( value );
			m_launch_flags |= LF_QUICKSTART_MAP_SIZE;
		}
	);
	const auto f_add_map_parameter_option =
		[ this, s_quickstart_argument_missing ]( const std::string& name, const std::string& desc, launch_flag_t flag, float* out_param )
			-> void {
			m_manager->AddRule(
				name, "PERCENTAGE", "Generate map with specific " + desc,
				AH( this, name, s_quickstart_argument_missing, out_param, &desc, flag ) {
					if ( !HasLaunchFlag( LF_QUICKSTART ) ) {
						Error( s_quickstart_argument_missing );
					}
					long int i = 0;
					if ( !util::String::ParseInt( value, i ) || i < 0 || i > 100 ) {
						Error( "Invalid --" + name + " value specified! Expected number ( 0 to 100 ), got: " + value );
					}
					*out_param = (float)i / 100;
					m_launch_flags |= flag;
				}
			);
		};
	f_add_map_parameter_option(
		"quickstart-map-ocean", "ocean coverage",
		LF_QUICKSTART_MAP_OCEAN, &m_quickstart_map_ocean_coverage
	);
	f_add_map_parameter_option(
		"quickstart-map-erosive", "erosive forces",
		LF_QUICKSTART_MAP_EROSIVE, &m_quickstart_map_erosive_forces
	);
	f_add_map_parameter_option(
		"quickstart-map-lifeforms", "native lifeforms",
		LF_QUICKSTART_MAP_LIFEFORMS, &m_quickstart_map_native_lifeforms
	);
	f_add_map_parameter_option(
		"quickstart-map-clouds", "cloud cover",
		LF_QUICKSTART_MAP_CLOUDS, &m_quickstart_map_cloud_cover
	);
	m_manager->AddRule(
		"quickstart-faction", "FACTION", "Play as specific faction", AH( this, s_quickstart_argument_missing ) {
			if ( !HasLaunchFlag( LF_QUICKSTART ) ) {
				Error( s_quickstart_argument_missing );
			}
			m_quickstart_faction = value;
			m_launch_flags |= LF_QUICKSTART_FACTION;
		}
	);
	m_manager->AddRule(
		"mods", "MODS", "Comma-separated list of mods to load", AH( this ) {
			std::stringstream ss( value );
			while ( ss.good() ) {
				std::string mod_name;
				getline( ss, mod_name, ',' );
				if ( !mod_name.empty() ) {
					const auto mod_path = util::FS::GeneratePath(
						{
							m_data_path,
							"mods",
							mod_name
						}
					);
					if ( !util::FS::DirectoryExists( mod_path ) ) {
						Error( "Mod path does not exist or is not a directory: " + mod_path );
					}
					m_mod_paths.push_back( mod_path );
				}
			}
			if ( m_mod_paths.empty() ) {
				Error( "No mod paths were defined" );
			}
			m_launch_flags |= LF_MODS;
		}
	);
	m_manager->AddRule(
		"username", "USERNAME", "Specify username for hosting or joining games", AH( this ) {
			m_launch_flags |= LF_USERNAME;
		}
	);
	m_manager->AddRule(
		"gamename", "GAMENAME", "Specify game name for hosting game", AH( this ) {
			m_launch_flags |= LF_GAMENAME;
		}
	);
	m_manager->AddRule(
		"host", "Host a TCP/IP game", AH( this ) {
			if ( HasLaunchFlag( LF_JOIN ) ) {
				Error( "Using --host with --join is not allowed" );
			}
			m_launch_flags |= LF_HOST;
		}
	);
	m_manager->AddRule(
		"join", "ADDRESS", "Join a TCP/IP game", AH( this ) {
			if ( HasLaunchFlag( LF_HOST ) ) {
				Error( "Using --host with --join is not allowed" );
			}
			m_launch_flags |= LF_JOIN;
			m_join_address = value;
		}
	);
	m_manager->AddRule(
		"legacy-ui", "Use legacy UI, development purposes only (broken, will be removed soon)", AH( this ) {
			m_launch_flags |= LF_LEGACY_UI;
		}
	);
	m_manager->AddRule(
		"mainscript", "SCRIPT_NAME", "Specify alternate main script to run (default: main)", AH( this ) {
			if ( HasLaunchFlag( LF_LEGACY_UI ) ) {
				Error( "--mainscript is not supported for legacy UI!" );
			}
			m_mainscript = value;
		}
	);

#if defined( DEBUG ) || defined( FASTDEBUG )
	m_manager->AddRule(
		"gdb", "Try to start within gdb (on supported platforms)", AH( this ) {
			m_debug_flags |= DF_GDB;
		}
	);
	m_manager->AddRule(
		"nopings", "Omit pings and timeouts during multiplayer games", AH( this ) {
			m_debug_flags |= DF_NOPINGS;
		}
	);
	m_manager->AddRule(
		"verbose-gc", "Output extra logs from garbage collector (spammy!)", AH( this ) {
			m_debug_flags |= DF_VERBOSE_GC;
		}
	);
	m_manager->AddRule(
		"no-gc", "Disable garbage collection (will leak memory!)", AH( this ) {
			m_debug_flags |= DF_NO_GC;
		}
	);
	m_manager->AddRule(
		"quiet", "Do not output debug logs to console", AH( this ) {
			m_debug_flags |= DF_QUIET;
		}
	);
	m_manager->AddRule(
		"gse-prompt-js", "Open interactive JS prompt", AH( this ) {
			m_debug_flags |= DF_GSE_ONLY | DF_GSE_PROMPT_JS;
		}
	);
	m_manager->AddRule(
		"gse-tests", "Run GSE tests and exit", AH( this ) {
			m_debug_flags |= DF_GSE_ONLY | DF_GSE_TESTS;
		}
	);
	m_manager->AddRule(
		"gse-tests-script", "SCRIPT_PATH", "Run only specific GSE test script", AH( this ) {
			if ( !HasDebugFlag( DF_GSE_TESTS ) ) {
				Error( "Gse-tests-related options can only be used after --gse-tests!" );
			}
			m_debug_flags |= DF_GSE_TESTS_SCRIPT;
			m_gse_tests_script = value;
		}
	);
	m_manager->AddRule(
		"single-thread", "Run everything in same thread", AH( this ) {
			m_debug_flags |= DF_SINGLE_THREAD;
		}
	);

#ifdef DEBUG
	m_manager->AddRule(
		"mapdump", "Save map dump upon loading map", AH( this ) {
			m_debug_flags |= DF_MAPDUMP;
		}
	);
	m_manager->AddRule(
		"memorydebug", "Add extra memory checks and tracking (slow!)", AH( this ) {
			m_debug_flags |= DF_MEMORYDEBUG;
		}
	);
	m_manager->AddRule(
		"quickstart-mapdump", "MAP_DUMP_FILE", "Load from existing map dump file (*.gsmd)", AH( this, s_quickstart_argument_missing ) {
			if ( !HasLaunchFlag( LF_QUICKSTART ) ) {
				Error( s_quickstart_argument_missing );
			}
			if ( !util::FS::FileExists( value ) ) {
				Error( "Map dump file \"" + value + "\" not found!" );
			}
			m_quickstart_mapdump = value;
			m_debug_flags |= DF_QUICKSTART_MAP_DUMP;
		}
	);
#endif

#endif

}

Config::~Config() {
	delete m_manager;
}

void Config::Init( const int argc, const char* argv[] ) {
	ASSERT( m_manager, "parser not set" );
	try {
		m_manager->ParseFile();
		m_manager->ParseArgs( argc, argv );
	}
	catch ( std::runtime_error& e ) {
		Error( e.what() );
	}
}

const std::string Config::GetEnv( const std::string& var ) const {
	const char* val = std::getenv( var.c_str() );
	if ( val == nullptr ) { // invalid to assign nullptr to std::string
		return "";
	}
	else {
		return val;
	}
}

const std::string& Config::GetPrefix() const {
	return m_prefix;
}

#if defined( DEBUG ) || defined( FASTDEBUG )

const std::string Config::GetDebugPath() const {
	return m_prefix + "debug/";
}

#endif

const std::string& Config::GetDataPath() const {
	return m_data_path;
}

const std::vector< std::string > Config::GetPossibleSMACPaths() const {
	std::vector< std::string > result = {};
	if ( !m_smac_path.empty() ) {
		result.push_back( m_smac_path );
	}
	if ( m_smac_path != "." ) {
		result.push_back( "." );
	}
	return result;
}

const smac_type_t Config::GetSMACType() const {
	return m_smac_type;
}

void Config::SetSMACPath( const std::string& path ) const {
	m_manager->UpdateSetting( "smacpath", path );
}

const bool Config::HasLaunchFlag( launch_flag_t flag ) const {
	return m_launch_flags & flag;
}

const types::Vec2< size_t >& Config::GetWindowSize() const {
	return m_window_size;
}

const util::random::state_t& Config::GetQuickstartSeed() const {
	return m_quickstart_seed;
}

const std::string& Config::GetQuickstartMapFile() const {
	return m_quickstart_mapfile;
}

const types::Vec2< size_t >& Config::GetQuickstartMapSize() const {
	return m_quickstart_mapsize;
}

const float Config::GetQuickstartMapOceanCoverage() const {
	return m_quickstart_map_ocean_coverage;
}

const float Config::GetQuickstartMapErosiveForces() const {
	return m_quickstart_map_erosive_forces;
}

const float Config::GetQuickstartMapNativeLifeforms() const {
	return m_quickstart_map_native_lifeforms;
}

const float Config::GetQuickstartMapCloudCover() const {
	return m_quickstart_map_cloud_cover;
}

const std::string& Config::GetQuickstartFaction() const {
	return m_quickstart_faction;
}

const std::vector< std::string >& Config::GetModPaths() const {
	return m_mod_paths;
}

const std::string& Config::GetJoinAddress() const {
	return m_join_address;
}

const std::string& Config::GetMainScript() const {
	return m_mainscript;
}

#if defined( DEBUG ) || defined( FASTDEBUG )

const bool Config::HasDebugFlag( const debug_flag_t flag ) const {
	return m_debug_flags & flag;
}

const std::string& Config::GetQuickstartMapDump() const {
	return m_quickstart_mapdump;
}

const std::string& Config::GetGSETestsScript() const {
	return m_gse_tests_script;
}

#endif

}
