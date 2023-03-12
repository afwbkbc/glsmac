#include <stdexcept>
#include <iostream>

#include "Config.h"

#include "version.h"

#include "util/SMACChecker.h"
#include "util/ArgParser.h"
#include "util/FS.h"

namespace config {

Config::Config( const int argc, const char *argv[] )
	: m_argc( argc ) 
	, m_argv( argv )
{
	util::ArgParser parser( argc, argv );
	
	const auto f_error = [ &parser ] ( const std::string& error ) -> void {
		std::cout << std::endl << "ERROR: " << error << std::endl << std::endl << parser.GetUnknownArgumentNote() << std::endl << std::endl;
		exit( EXIT_FAILURE );
	};
	
	const std::string s_invalid_smac_directory = " is not valid SMAC directory!\n\tRun from SMAC directory or pass it with --smacpath argument";
	
	parser.AddRule( "benchmark", "Disable VSync and FPS limit, show FPS counter at top left corner", AH( this ) {
		m_launch_flags |= LF_BENCHMARK;
	});
	parser.AddRule( "help", "Show this message", AH( this, &parser ) {
		std::cout << parser.GetHelpString() << std::endl;
		exit( EXIT_SUCCESS );
	});
	parser.AddRule( "smacpath", "SMAC_PATH", "Specify path to SMAC directory (must contain expansion too)", AH( this, f_error, s_invalid_smac_directory ) {
		if ( !util::SMACChecker::IsSMACDirectory( value ) ) {
			f_error( (std::string) value + s_invalid_smac_directory );
		}
		m_smac_path = value + "/";
	});
	parser.AddRule( "version", "Show version of GLSMAC", AH() {
		std::cout
				<< std::endl
			<< GLSMAC_VERSION_FULL << ' ' << GLSMAC_URL << std::endl
				<< std::endl
			<< GLSMAC_LICENSE << std::endl
				<< std::endl
		;
		exit( EXIT_SUCCESS );
	});

#ifdef DEBUG
	parser.AddRule( "gdb", "Try to start within gdb (on supported platforms)", AH( this ) {
		m_debug_flags |= DF_GDB;
	});
	parser.AddRule( "memorydebug", "Add extra memory checks and tracking (slow!)", AH( this ) {
		m_debug_flags |= DF_MEMORYDEBUG;
	});
	parser.AddRule( "quickstart", "Skip intro and main menu and generate/load map directly", AH( this ) {
		m_debug_flags |= DF_QUICKSTART;
	});
	const std::string s_quickstart_argument_missing = "Quickstart-related options can only be used after --quickstart argument!";
	parser.AddRule( "quickstart-seed", "SEED", "Generate map with specific seed (A:B:C:D)", AH( this, f_error, &parser, s_quickstart_argument_missing ) {
		if ( !HasDebugFlag( DF_QUICKSTART ) ) {
			f_error( s_quickstart_argument_missing );
		}
		try {
			m_quickstart_seed = util::Random::GetStateFromString( value );
		} catch ( std::runtime_error& e ) {
			f_error( "Invalid seed format! Seed must contain four numbers separated by colon, for example: 1651011033:1377505029:3019448108:3247278135" );
		}
		m_debug_flags |= DF_QUICKSTART_SEED;
	});
	parser.AddRule( "quickstart-mapdump", "MAP_DUMP_FILE", "Load from existing map dump file (*.gsmd)", AH( this, f_error, s_quickstart_argument_missing ) {
		if ( !HasDebugFlag( DF_QUICKSTART ) ) {
			f_error( s_quickstart_argument_missing );
		}
		if ( !util::FS::FileExists( value ) ) {
			f_error( "Map dump file \"" + value + "\" not found!" );
		}
		m_quickstart_mapdump = value;
		m_debug_flags |= DF_QUICKSTART_MAPDUMP;
	});
	parser.AddRule( "quickstart-mapfile", "MAP_FILE", "Load from existing map file (*.gsm)", AH( this, f_error, s_quickstart_argument_missing ) {
		if ( !HasDebugFlag( DF_QUICKSTART ) ) {
			f_error( s_quickstart_argument_missing );
		}
		if ( !util::FS::FileExists( value ) ) {
			f_error( "Map file \"" + value + "\" not found!" );
		}
		m_quickstart_mapfile = value;
		m_debug_flags |= DF_QUICKSTART_MAPFILE;
	});
	parser.AddRule( "quickstart-mapsize", "MAP_SIZE", "Generate map of specific size (WxH)", AH( this, f_error, s_quickstart_argument_missing ) {
		if ( !HasDebugFlag( DF_QUICKSTART ) ) {
			f_error( s_quickstart_argument_missing );
		}
		const size_t pos = value.find( 'x' );
		const std::string s_invalid_format = "Invalid size specified! Format is WIDTHxHEIGHT, for example: 80x40";
		if ( pos == std::string::npos ) {
			f_error( s_invalid_format );
		}
		try {
			m_quickstart_mapsize.x = std::stoul( value.substr( 0, pos ) );
			m_quickstart_mapsize.y = std::stoul( value.substr( pos + 1 ) );
		} catch ( std::invalid_argument& e ) {
			f_error( s_invalid_format );
		}
		m_debug_flags |= DF_QUICKSTART_MAPSIZE;
	});
#endif
	
	try {
		parser.Parse();
	} catch ( std::runtime_error& e ) {
		f_error( e.what() );
	}
	
	if ( m_smac_path.empty() ) {
		if ( !util::SMACChecker::IsSMACDirectory( "." ) ) {
			f_error( "This" + s_invalid_smac_directory );
		}
		m_smac_path = "./";
	}
	
}

const std::string& Config::GetSMACPath() const {
	return m_smac_path;
}

const bool Config::HasLaunchFlag( launch_flag_t flag ) const {
	return m_launch_flags & flag;
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

#endif

}
