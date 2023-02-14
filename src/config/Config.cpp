#include <stdexcept>
#include <iostream>

#include "Config.h"

#include "util/SMACChecker.h"

namespace config {

Config::Config( const int argc, const char *argv[] )
	: m_argc( argc ) 
	, m_argv( argv )
{
	try {
		if ( util::SMACChecker::IsSMACDirectory( "." ) ) {
			m_smac_path = "./";
		}
		else if (m_argc == 2) {
			if (util::SMACChecker::IsSMACDirectory(m_argv[1])) {
				m_smac_path = (std::string)m_argv[1] + "/";
			}
			else {
				throw std::runtime_error( (std::string) m_argv[1] + " does not seem to be valid SMAC directory." );
			}
		}
		else {
			throw std::runtime_error( (std::string) "SMAC directory not found. Either place GLSMAC into SMAC directory or specify it from command line ( " + m_argv[0] + " <path_to_SMAC_directory> )." );
		}
	}
	catch (std::exception &e) {
		// something wrong in parameters or configs
		std::cout << std::endl << e.what() << std::endl << std::endl;
		exit(EXIT_FAILURE);
	}
}

}
