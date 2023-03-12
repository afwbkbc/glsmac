#include "ArgParser.h"

namespace util {

ArgParser::ArgParser( const int argc, const char *argv[] )
	: m_argc( argc ) 
	, m_argv( argv )
{
	
}

void ArgParser::AddRule( const std::string& argument, const std::string& description, const arg_handler_t handler ) {
	m_arg_rules[ argument ] = {
		argument,
		false,
		"",
		description,
		handler
	};
	size_t sz = argument.size() + 6;
	while ( m_offset < sz ) {
		m_offset += m_offset_step;
	}
}

void ArgParser::AddRule( const std::string& argument, const std::string& value_name, const std::string& description, const arg_handler_t handler ) {
	m_arg_rules[ argument ] = {
		argument,
		true,
		value_name,
		description,
		handler
	};
	size_t sz = argument.size() + value_name.size() + 6;
	while ( m_offset < sz ) {
		m_offset += m_offset_step;
	}
}


const std::string ArgParser::GetHelpString() const {
	std::string result = (std::string) "\n" +
		"Usage:\n\t" + m_argv[ 0 ] + " [options]\n" +
		"\n" +
		"Options:\n";
	for ( auto& rule : m_arg_rules ) {
		result +=
				std::string( m_offset_step, ' ' ) +
			"--" + rule.first +
				' ' +
			rule.second.value_name +
				std::string( std::max<ssize_t>( 1, (ssize_t)m_offset - rule.first.size() - rule.second.value_name.size() - 4 ), ' ' ) +
			rule.second.description
		+ "\n";
	}
	return result;
}

const std::string ArgParser::GetUnknownArgumentNote() const {
	return (std::string) "Run\n\t" + m_argv[ 0 ] + " --help\nfor list of supported options.";
}

void ArgParser::Parse() {
	ASSERT( m_argc >= 1, "argc is zero" );
	
	const char *c;
	std::string kv, k, v;
	for ( int i = 1 ; i < m_argc ; i++ ) {
		c = m_argv[ i ];
		if ( c[ 0 ] != '-' || c[ 1 ] != '-' ) {
			throw std::runtime_error( (std::string) "Unknown argument \"" + c + "\"!" );
		}
		kv = ( c + 2 );
		size_t splitpos = kv.find( '=' );
		if ( splitpos != std::string::npos ) {
			k = kv.substr( 0, splitpos );
			v = kv.substr( splitpos + 1 );
		}
		else {
			k = kv;
			if (
				( i < m_argc - 1 )
			) {
				v = m_argv[ i + 1 ];
			}
			else {
				v = "";
			}
		}
		const auto& it = m_arg_rules.find( k );
		if ( it == m_arg_rules.end() ) {
			throw std::runtime_error( (std::string) "Unknown argument --" + k + "!" );
		}
		if ( it->second.has_value && i == m_argc - 1 ) {
			throw std::runtime_error( (std::string) "--" + k + " requires a value!" );
		}
		it->second.handler( v );
		if ( it->second.has_value ) {
			i++;
		}
	}
}

}
