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
		if (m_argc != 2) {
			throw std::runtime_error( (std::string) "Usage: " + m_argv[0] + " [path_to_SMAC_directory]" );
		}
		m_game_path = (std::string) m_argv[1] + "/";
		util::SMACChecker::CheckPath(m_game_path);
	}
	catch (std::exception &e) {
		// something wrong in parameters or configs
		std::cout << std::endl << e.what() << std::endl << std::endl;
		exit(EXIT_FAILURE);
	}
}

}
