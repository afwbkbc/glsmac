#include <iostream>
#include <stdexcept>

#include "Config.h"

#include "version.h"

#include "util/ArgParser.h"
#include "util/FS.h"
#include "util/random/Random.h"

#include "game/settings/Settings.h"

namespace config {

void Config::Error( const std::string& error ) {
	std::cout << std::endl
		<< "ERROR: " << error << std::endl
		<< std::endl
		<< m_parser->GetUnknownArgumentNote() << std::endl
		<< std::endl;
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

Config::Config( const int argc, const char* argv[] )
	: m_smac_path( "" )
	, m_prefix( DEFAULT_GLSMAC_PREFIX + util::FS::PATH_SEPARATOR ) {
	
	m_parser = new util::ArgParser( argc, argv );

	m_parser->AddRule(
		"benchmark", "Disable VSync and FPS limit", AH( this ) {
			m_launch_flags |= LF_BENCHMARK;
		}
	);
	m_parser->AddRule(
		"showfps", "Show FPS counter at top left corner", AH( this ) {
			m_launch_flags |= LF_SHOWFPS;
		}
	);
	m_parser->AddRule(
		"help", "Show this message", AH( this ) {
			std::cout << m_parser->GetHelpString() << std::endl;
			exit( EXIT_SUCCESS );
		}
	);
	m_parser->AddRule(
		"nosound", "Start without sound", AH( this ) {
			m_launch_flags |= LF_NOSOUND;
		}
	);
	m_parser->AddRule(
		"prefix", "GLSMAC_PREFIX", "Path to store GLSMAC data in (default: " + DEFAULT_GLSMAC_PREFIX + ")", AH( this ) {
			m_prefix = value + util::FS::PATH_SEPARATOR;
		}
	);
	m_parser->AddRule(
		"skipintro", "Skip intro", AH( this ) {
			m_launch_flags |= LF_SKIPINTRO;
		}
	);
	m_parser->AddRule(
		"smacpath", "SMAC_PATH", "Specify path to SMAC directory (must contain SMACX too)", AH( this ) {
			m_smac_path = value;
		}
	);
	m_parser->AddRule(
		"version", "Show version of GLSMAC", AH() {
			std::cout
				<< std::endl
				<< GLSMAC_VERSION_FULL << ' ' << GLSMAC_URL << std::endl
				<< std::endl
				<< GLSMAC_LICENSE << std::endl
				<< std::endl;
			exit( EXIT_SUCCESS );
		}
	);
	m_parser->AddRule(
		"windowed", "Start in windowed mode", AH( this ) {
			m_launch_flags |= LF_WINDOWED;
		}
	);
	m_parser->AddRule(
		"window-size", "WINDOW_SIZE", "Size of window", AH( this ) {
			if ( !HasLaunchFlag( LF_WINDOWED ) ) {
				Error( "Window-related options can only be used after --windowed argument!" );
			}
			m_window_size = ParseSize( value );
			m_launch_flags |= LF_WINDOW_SIZE;
		}
	);

#ifdef DEBUG
	m_parser->AddRule(
		"gdb", "Try to start within gdb (on supported platforms)", AH( this ) {
			m_debug_flags |= DF_GDB;
		}
	);
	m_parser->AddRule(
		"mapdump", "Save map dump upon loading map", AH( this ) {
			m_debug_flags |= DF_MAPDUMP;
		}
	);
	m_parser->AddRule(
		"memorydebug", "Add extra memory checks and tracking (slow!)", AH( this ) {
			m_debug_flags |= DF_MEMORYDEBUG;
		}
	);
	m_parser->AddRule(
		"nopings", "Omit pings and timeouts during multiplayer games", AH( this ) {
			m_debug_flags |= DF_NOPINGS;
		}
	);
	m_parser->AddRule(
		"quickstart", "Skip intro and main menu and generate/load map directly", AH( this ) {
			m_debug_flags |= DF_QUICKSTART;
		}
	);
	const std::string s_quickstart_argument_missing = "Quickstart-related options can only be used after --quickstart argument!";
	m_parser->AddRule(
		"quickstart-seed", "SEED", "Generate map with specific seed (A:B:C:D)", AH( this, s_quickstart_argument_missing ) {
			if ( !HasDebugFlag( DF_QUICKSTART ) ) {
				Error( s_quickstart_argument_missing );
			}
			try {
				m_quickstart_seed = util::random::Random::GetStateFromString( value );
			}
			catch ( std::runtime_error& e ) {
				Error( "Invalid seed format! Seed must contain four numbers separated by colon, for example: 1651011033:1377505029:3019448108:3247278135" );
			}
			m_debug_flags |= DF_QUICKSTART_SEED;
		}
	);
	m_parser->AddRule(
		"quickstart-mapdump", "MAP_DUMP_FILE", "Load from existing map dump file (*.gsmd)", AH( this, s_quickstart_argument_missing ) {
			if ( !HasDebugFlag( DF_QUICKSTART ) ) {
				Error( s_quickstart_argument_missing );
			}
			if ( !util::FS::FileExists( value ) ) {
				Error( "Map dump file \"" + value + "\" not found!" );
			}
			m_quickstart_mapdump = value;
			m_debug_flags |= DF_QUICKSTART_MAP_DUMP;
		}
	);
	m_parser->AddRule(
		"quickstart-mapfile", "MAP_FILE", "Load from existing map file (*.gsm)", AH( this, s_quickstart_argument_missing ) {
			if ( !HasDebugFlag( DF_QUICKSTART ) ) {
				Error( s_quickstart_argument_missing );
			}
			if ( !util::FS::FileExists( value ) ) {
				Error( "Map file \"" + value + "\" not found!" );
			}
			m_quickstart_mapfile = value;
			m_debug_flags |= DF_QUICKSTART_MAP_FILE;
		}
	);
	m_parser->AddRule(
		"quickstart-mapsize", "MAP_SIZE", "Generate map of specific size (WxH)", AH( this, s_quickstart_argument_missing ) {
			if ( !HasDebugFlag( DF_QUICKSTART ) ) {
				Error( s_quickstart_argument_missing );
			}
			m_quickstart_mapsize = ParseSize( value );
			m_debug_flags |= DF_QUICKSTART_MAP_SIZE;
		}
	);
	const auto f_add_map_parameter_option =
		[ this, s_quickstart_argument_missing ]( const std::string& name, const std::vector< std::string >& values, const std::string& desc, debug_flag_t flag, game::settings::map_config_value_t* out_param )
			-> void {
			ASSERT( values.size() == 3, "values size mismatch" );
			m_parser->AddRule(
				name, values[ 0 ] + "|" + values[ 1 ] + "|" + values[ 2 ], "Generate map with specific " + desc + " setting",
				AH( this, name, values, s_quickstart_argument_missing, out_param, &desc, flag ) {
					if ( !HasDebugFlag( DF_QUICKSTART ) ) {
						Error( s_quickstart_argument_missing );
					}
					if ( value == values[ 0 ] ) {
						*out_param = 1;
					}
					else if ( value == values[ 1 ] ) {
						*out_param = 2;
					}
					else if ( value == values[ 2 ] ) {
						*out_param = 3;
					}
					else {
						Error( "Invalid --" + name + " value specified! Possible choices: " + values[ 0 ] + " " + values[ 1 ] + " " + values[ 2 ] );
					}
					m_debug_flags |= flag;
				}
			);
		};
	f_add_map_parameter_option(
		"quickstart-map-ocean", {
			"low",
			"medium",
			"high"
		}, "ocean coverage",
		DF_QUICKSTART_MAP_OCEAN, &m_quickstart_map_ocean
	);
	f_add_map_parameter_option(
		"quickstart-map-erosive", {
			"strong",
			"average",
			"weak"
		}, "erosive forces",
		DF_QUICKSTART_MAP_EROSIVE, &m_quickstart_map_erosive
	);
	f_add_map_parameter_option(
		"quickstart-map-lifeforms", {
			"rare",
			"average",
			"abundant"
		}, "native lifeforms",
		DF_QUICKSTART_MAP_LIFEFORMS, &m_quickstart_map_lifeforms
	);
	f_add_map_parameter_option(
		"quickstart-map-clouds", {
			"sparse",
			"average",
			"dense"
		}, "cloud cover",
		DF_QUICKSTART_MAP_CLOUDS, &m_quickstart_map_clouds
	);
	m_parser->AddRule(
		"quiet", "Do not output debug logs to console", AH( this ) {
			m_debug_flags |= DF_QUIET;
		}
	);
	m_parser->AddRule(
		"gse-prompt-js", "Open interactive JS prompt", AH( this ) {
			m_debug_flags |= DF_GSE_ONLY | DF_GSE_PROMPT_JS;
		}
	);
	m_parser->AddRule(
		"gse-tests", "Run GSE tests and exit", AH( this ) {
			m_debug_flags |= DF_GSE_ONLY | DF_GSE_TESTS;
		}
	);
	m_parser->AddRule(
		"gse-tests-script", "SCRIPT_PATH", "Run only specific GSE test script", AH( this ) {
			if ( !HasDebugFlag( DF_GSE_TESTS ) ) {
				Error( "Gse-tests-related options can only be used after --gse-tests!" );
			}
			m_debug_flags |= DF_GSE_TESTS_SCRIPT;
			m_gse_tests_script = value;
		}
	);

#endif

}

Config::~Config() {
	if ( m_parser ) {
		delete m_parser;
	}
}

void Config::Init() {
	ASSERT( m_parser, "parser not set" );
	try {
		m_parser->Parse();
	}
	catch ( std::runtime_error& e ) {
		Error( e.what() );
	}

	delete m_parser; // all done
	m_parser = nullptr;
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

#ifdef DEBUG

const std::string Config::GetDebugPath() const {
	return m_prefix + "debug/";
}

#endif

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

const bool Config::HasLaunchFlag( launch_flag_t flag ) const {
	return m_launch_flags & flag;
}

const types::Vec2< size_t >& Config::GetWindowSize() const {
	return m_window_size;
}

#ifdef DEBUG

const bool Config::HasDebugFlag( const debug_flag_t flag ) const {
	return m_debug_flags & flag;
}

const util::random::state_t& Config::GetQuickstartSeed() const {
	return m_quickstart_seed;
}

const std::string& Config::GetQuickstartMapDump() const {
	return m_quickstart_mapdump;
}

const std::string& Config::GetQuickstartMapFile() const {
	return m_quickstart_mapfile;
}

const types::Vec2< size_t >& Config::GetQuickstartMapSize() const {
	return m_quickstart_mapsize;
}

const game::settings::map_config_value_t Config::GetQuickstartMapOcean() const {
	return m_quickstart_map_ocean;
}

const game::settings::map_config_value_t Config::GetQuickstartMapErosive() const {
	return m_quickstart_map_erosive;
}

const game::settings::map_config_value_t Config::GetQuickstartMapLifeforms() const {
	return m_quickstart_map_lifeforms;
}

const game::settings::map_config_value_t Config::GetQuickstartMapClouds() const {
	return m_quickstart_map_clouds;
}

const std::string& Config::GetGSETestsScript() const {
	return m_gse_tests_script;
}

#endif

}
