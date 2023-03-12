#include <stdexcept>
#include <iostream>

#include "Config.h"

#include "util/SMACChecker.h"
#include "util/ArgParser.h"

namespace config {

Config::Config( const int argc, const char *argv[] )
	: m_argc( argc ) 
	, m_argv( argv )
{
	const auto f_error = [] ( const std::string& error ) -> void {
		std::cout << std::endl << "ERROR: " << error << std::endl << std::endl;
		exit( EXIT_FAILURE );
	};
	
	util::ArgParser parser( argc, argv );
	
	const std::string s_invalid_smac_directory = " is not valid SMAC directory!\n\tRun from SMAC directory or pass it with --smacpath argument.\n" + parser.GetUnknownArgumentNote();
	
	parser.AddRule( "help", "Show this message", AH( this, &parser ) {
		std::cout << parser.GetHelpString() << std::endl;
		exit( EXIT_SUCCESS );
	});
	parser.AddRule( "smacpath", "SMAC_PATH", "Path to SMAC directory (must contain expansion too)", AH( this, f_error, s_invalid_smac_directory ) {
		if ( !util::SMACChecker::IsSMACDirectory( value ) ) {
			f_error( (std::string) value + s_invalid_smac_directory );
		}
		m_smac_path = value + "/";
	});
	parser.AddRule( "benchmark", "Disables VSync and FPS limit, shows FPS counter at top left corner", AH( this ) {
		m_launch_flags |= LF_BENCHMARK;
	});
	
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

}
