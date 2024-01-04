#include <iostream>
#include <stdexcept>

#include "Config.h"

#include "version.h"

#include "util/ArgParser.h"
#include "util/FS.h"
#include "util/SMACChecker.h"

#include "game/Settings.h"

namespace config {

Config::Config( const int argc, const char* argv[] ) {
	util::ArgParser parser( argc, argv );

	const auto f_error = [ &parser ]( const std::string& error ) -> void {
		std::cout << std::endl
			<< "ERROR: " << error << std::endl
			<< std::endl
			<< parser.GetUnknownArgumentNote() << std::endl
			<< std::endl;
		exit( EXIT_FAILURE );
	};

	const auto f_parse_size = [ f_error ]( const std::string& value ) -> types::Vec2< size_t > {
		const size_t pos = value.find( 'x' );
		const std::string s_invalid_format = "Invalid size specified! Format is WIDTHxHEIGHT, for example: 80x40";
		if ( pos == std::string::npos ) {
			f_error( s_invalid_format );
		}
		types::Vec2< size_t > result = {};
		try {
			result.x = std::stoul( value.substr( 0, pos ) );
			result.y = std::stoul( value.substr( pos + 1 ) );
		}
		catch ( std::invalid_argument& e ) {
			f_error( s_invalid_format );
		}
		return result;
	};

	const std::string s_invalid_smac_directory = " is not valid SMAC directory!\n\tRun from SMAC directory or pass it with --smacpath argument";

	parser.AddRule(
		"benchmark", "Disable VSync and FPS limit, show FPS counter at top left corner", AH( this ) {
			m_launch_flags |= LF_BENCHMARK;
		}
	);
	parser.AddRule(
		"help", "Show this message", AH( this, &parser ) {
			std::cout << parser.GetHelpString() << std::endl;
			exit( EXIT_SUCCESS );
		}
	);
	parser.AddRule(
		"nosound", "Start without sound", AH( this ) {
			m_launch_flags |= LF_NOSOUND;
		}
	);
	parser.AddRule(
		"prefix", "GLSMAC_PREFIX", "Path to store GLSMAC data in (default: " + DEFAULT_GLSMAC_PREFIX + ")", AH( this ) {
			m_prefix = value + "/";
		}
	);
	parser.AddRule(
		"skipintro", "Skip intro", AH( this ) {
			m_launch_flags |= LF_SKIPINTRO;
		}
	);
	parser.AddRule(
		"smacpath", "SMAC_PATH", "Specify path to SMAC directory (must contain SMACX too)", AH( this, f_error, s_invalid_smac_directory ) {
			if ( !util::SMACChecker::IsSMACDirectory( value ) ) {
				f_error( (std::string)value + s_invalid_smac_directory );
			}
			m_smac_path = value + "/";
		}
	);
	parser.AddRule(
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
	parser.AddRule(
		"windowed", "Start in windowed mode", AH( this ) {
			m_launch_flags |= LF_WINDOWED;
		}
	);
	parser.AddRule(
		"window-size", "WINDOW_SIZE", "Size of window", AH( this, f_error, f_parse_size ) {
			if ( !HasLaunchFlag( LF_WINDOWED ) ) {
				f_error( "Window-related options can only be used after --windowed argument!" );
			}
			m_window_size = f_parse_size( value );
			m_launch_flags |= LF_WINDOW_SIZE;
		}
	);

#ifdef DEBUG
	parser.AddRule(
		"gdb", "Try to start within gdb (on supported platforms)", AH( this ) {
			m_debug_flags |= DF_GDB;
		}
	);
	parser.AddRule(
		"mapdump", "Save map dump upon loading map", AH( this ) {
			m_debug_flags |= DF_MAPDUMP;
		}
	);
	parser.AddRule(
		"memorydebug", "Add extra memory checks and tracking (slow!)", AH( this ) {
			m_debug_flags |= DF_MEMORYDEBUG;
		}
	);
	parser.AddRule(
		"quickstart", "Skip intro and main menu and generate/load map directly", AH( this ) {
			m_debug_flags |= DF_QUICKSTART;
		}
	);
	const std::string s_quickstart_argument_missing = "Quickstart-related options can only be used after --quickstart argument!";
	parser.AddRule(
		"quickstart-seed", "SEED", "Generate map with specific seed (A:B:C:D)", AH( this, f_error, &parser, s_quickstart_argument_missing ) {
			if ( !HasDebugFlag( DF_QUICKSTART ) ) {
				f_error( s_quickstart_argument_missing );
			}
			try {
				m_quickstart_seed = util::Random::GetStateFromString( value );
			}
			catch ( std::runtime_error& e ) {
				f_error( "Invalid seed format! Seed must contain four numbers separated by colon, for example: 1651011033:1377505029:3019448108:3247278135" );
			}
			m_debug_flags |= DF_QUICKSTART_SEED;
		}
	);
	parser.AddRule(
		"quickstart-mapdump", "MAP_DUMP_FILE", "Load from existing map dump file (*.gsmd)", AH( this, f_error, s_quickstart_argument_missing ) {
			if ( !HasDebugFlag( DF_QUICKSTART ) ) {
				f_error( s_quickstart_argument_missing );
			}
			if ( !util::FS::FileExists( value ) ) {
				f_error( "Map dump file \"" + value + "\" not found!" );
			}
			m_quickstart_mapdump = value;
			m_debug_flags |= DF_QUICKSTART_MAP_DUMP;
		}
	);
	parser.AddRule(
		"quickstart-mapfile", "MAP_FILE", "Load from existing map file (*.gsm)", AH( this, f_error, s_quickstart_argument_missing ) {
			if ( !HasDebugFlag( DF_QUICKSTART ) ) {
				f_error( s_quickstart_argument_missing );
			}
			if ( !util::FS::FileExists( value ) ) {
				f_error( "Map file \"" + value + "\" not found!" );
			}
			m_quickstart_mapfile = value;
			m_debug_flags |= DF_QUICKSTART_MAP_FILE;
		}
	);
	parser.AddRule(
		"quickstart-mapsize", "MAP_SIZE", "Generate map of specific size (WxH)", AH( this, s_quickstart_argument_missing, f_parse_size, f_error ) {
			if ( !HasDebugFlag( DF_QUICKSTART ) ) {
				f_error( s_quickstart_argument_missing );
			}
			m_quickstart_mapsize = f_parse_size( value );
			m_debug_flags |= DF_QUICKSTART_MAP_SIZE;
		}
	);
	const auto f_add_map_parameter_option =
		[ this, &parser, f_error, &s_quickstart_argument_missing ]( const std::string& name, const std::vector< std::string >& values, const std::string& desc, debug_flag_t flag, game::MapSettings::parameter_t* out_param )
			-> void {
			ASSERT( values.size() == 3, "values size mismatch" );
			parser.AddRule(
				name, values[ 0 ] + "|" + values[ 1 ] + "|" + values[ 2 ], "Generate map with specific " + desc + " setting",
				AH( this, name, values, f_error, &s_quickstart_argument_missing, out_param, &desc, flag ) {
					if ( !HasDebugFlag( DF_QUICKSTART ) ) {
						f_error( s_quickstart_argument_missing );
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
						f_error( "Invalid --" + name + " value specified! Possible choices: " + values[ 0 ] + " " + values[ 1 ] + " " + values[ 2 ] );
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
	parser.AddRule(
		"quiet", "Do not output debug logs to console", AH( this ) {
			m_debug_flags |= DF_QUIET;
		}
	);
	parser.AddRule(
		"gse-prompt-js", "Open interactive JS prompt", AH( this ) {
			m_debug_flags |= DF_GSE_ONLY | DF_GSE_PROMPT_JS;
		}
	);
	parser.AddRule(
		"gse-tests", "Run GSE tests and exit", AH( this ) {
			m_debug_flags |= DF_GSE_ONLY | DF_GSE_TESTS;
		}
	);
#endif

	try {
		parser.Parse();
	}
	catch ( std::runtime_error& e ) {
		f_error( e.what() );
	}

#ifdef DEBUG
	if ( !( m_debug_flags & DF_GSE_ONLY ) )
#endif
	{
		if ( m_smac_path.empty() ) {
			if ( !util::SMACChecker::IsSMACDirectory( "." ) ) {
				f_error( "This" + s_invalid_smac_directory );
			}
			m_smac_path = "./";
		}
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

#ifdef DEBUG

const std::string Config::GetDebugPath() const {
	return m_prefix + "debug/";
}

#endif

const std::string& Config::GetSMACPath() const {
	return m_smac_path;
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

const util::Random::state_t& Config::GetQuickstartSeed() const {
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

const game::MapSettings::parameter_t Config::GetQuickstartMapOcean() const {
	return m_quickstart_map_ocean;
}

const game::MapSettings::parameter_t Config::GetQuickstartMapErosive() const {
	return m_quickstart_map_erosive;
}

const game::MapSettings::parameter_t Config::GetQuickstartMapLifeforms() const {
	return m_quickstart_map_lifeforms;
}

const game::MapSettings::parameter_t Config::GetQuickstartMapClouds() const {
	return m_quickstart_map_clouds;
}

#endif

}
