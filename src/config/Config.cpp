#include <stdexcept>
#include <iostream>

#include "Config.h"

#include "util/SMACChecker.h"

namespace config {

Config::Config( const int argc, const char *argv[] )
	: m_argc( argc ) 
	, m_argv( argv )
{
	ASSERT( m_argc >= 1, "argc is zero" );
	const auto f_error = [] ( const std::string& error ) -> void {
		std::cout << std::endl << "ERROR: " << error << std::endl << std::endl;
		exit( EXIT_FAILURE );
	};
	const auto f_usage = [ this ] () -> void {
		// show usage
		std::cout << std::endl
			<< "Usage:\n\t" << m_argv[ 0 ] << " [options]" << std::endl
				<< std::endl
			<< "Options:" << std::endl
			<< "\t--help\t\t\tShow this message" << std::endl
			<< "\t--smacpath SMAC_PATH\tPath to SMAC directory (must contain expansion too)" << std::endl
			<< "\t--benchmark\t\tDisables VSync and FPS limit, shows FPS counter at top left corner" << std::endl
		<< std::endl;
	};
	
	const std::string s_help_for_more_options = (std::string) "\nRun\n\t" + m_argv[ 0 ] + " --help\nfor list of supported options.";
	const std::string s_invalid_smac_directory = " is not valid SMAC directory!\n\tRun from SMAC directory or pass it with --smacpath argument." + s_help_for_more_options;
	
	const char *c;
	std::string kv, k, v, vn;
	bool n;
	for ( int i = 1 ; i < m_argc ; i++ ) {
		c = m_argv[ i ];
		if ( c[ 0 ] != '-' || c[ 1 ] != '-' ) {
			f_error( (std::string) "Unknown argument \"" + c + "\"." + s_help_for_more_options );
		}
		kv = ( c + 2 );
		size_t splitpos = kv.find( '=' );
		if ( splitpos != std::string::npos ) {
			k = kv.substr( 0, splitpos );
			v = kv.substr( splitpos + 1 );
			n = false;
		}
		else {
			k = kv;
			if (
				( i < m_argc - 1 )
			) {
				v = m_argv[ i + 1 ];
				n = true;
			}
			else {
				v = "";
			}
		}
		
		// parse arguments
		
		if ( k == "help" ) {
			f_usage();
			exit( EXIT_SUCCESS );
		}
		else if ( k == "smacpath" ) {
			if ( !util::SMACChecker::IsSMACDirectory( v ) ) {
				f_error( (std::string) v + s_invalid_smac_directory );
			}
			m_smac_path = v + "/";
		}
		else if ( k == "benchmark" ) {
			m_launch_flags |= LF_BENCHMARK;
			n = false;
		}
		else {
			f_error( (std::string) "Unknown argument --" + k + "." + s_help_for_more_options );
		}
		if ( n ) {
			i++;
		}
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
