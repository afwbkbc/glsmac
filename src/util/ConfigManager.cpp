#include "ConfigManager.h"

#include <yaml-cpp/yaml.h>

#include "util/FS.h"
#include "util/String.h"

namespace util {

ConfigManager::ConfigManager( const std::string& path, const std::string& config_path )
	: m_path( path )
	, m_config_path( config_path ) {}

void ConfigManager::AddRule( const std::string& argument, const std::string& description, const arg_handler_t handler ) {
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

void ConfigManager::AddRule( const std::string& argument, const std::string& value_name, const std::string& description, const arg_handler_t handler ) {
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

const std::string ConfigManager::GetHelpString() const {
	std::string result = (std::string)"\n" +
		"Usage:\n\t" + m_path + " [options]\n" +
		"\n" +
		"Options:\n";
	for ( auto& rule : m_arg_rules ) {
		result +=
			std::string( m_offset_step, ' ' ) +
				"--" + rule.first +
				' ' +
				rule.second.value_name +
				std::string( std::max< ssize_t >( 1, (ssize_t)m_offset - rule.first.size() - rule.second.value_name.size() - 4 ), ' ' ) +
				rule.second.description
				+ "\n";
	}
	return result;
}

const std::string ConfigManager::GetUnknownArgumentNote() const {
	return (std::string)"Run\n\t" + m_path + " --help\nfor list of supported options.";
}

static const std::string s_errprefix_args = "argument --";
void ConfigManager::ParseArgs( const int argc, const char* argv[] ) {
	ASSERT( argc >= 1, "argc is zero" );

	const char* c;
	std::string kv, k;
	std::optional< std::string > v;
	for ( int i = 1 ; i < argc ; i++ ) {
		c = argv[ i ];
		if ( c[ 0 ] != '-' || c[ 1 ] != '-' ) {
			throw std::runtime_error( (std::string)"unknown argument \"" + c + "\"!" );
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
				( i < argc - 1 )
				) {
				v = argv[ i + 1 ];
			}
			else {
				v = {};
			}
		}
		const auto& it = Set( k, v, s_errprefix_args, false );
		if ( it->second.has_value ) {
			i++;
		}
	}
}

void ConfigManager::ParseFile( const std::string& path ) {
	const auto& p = path.empty()
		? m_config_path
		: path;
	if ( util::FS::FileExists( p ) ) {
		const auto errprefix = "setting " + p + ": ";
		YAML::Node root = YAML::LoadFile( p );
		for ( const auto& it : root ) {
			const auto& k = it.first.as< std::string >();
			if ( !it.second.IsNull() ) {
				Set( k, it.second.as< std::string >(), errprefix, true );
			}
			else {
				Set( k, {}, errprefix, true );
			}
		}
	}
}

void ConfigManager::UpdateSetting( const std::string& key, const std::string& value ) {
	m_settings.insert_or_assign( key, std::make_pair( true, value ) );
	Save();
}

const std::map< std::string, ConfigManager::arg_rule_t >::iterator ConfigManager::Set( const std::string& k, const std::optional< std::string >& v, const std::string& err_prefix, const bool is_saveable ) {
	const auto& it = m_arg_rules.find( k );
	if ( it == m_arg_rules.end() ) {
		throw std::runtime_error( (std::string)"invalid " + err_prefix + k );
	}
	if ( it->second.has_value && !v.has_value() ) {
		throw std::runtime_error( (std::string)err_prefix + k + " requires a value!" );
	}
	it->second.handler( v.value_or( "" ) );
	m_settings.insert_or_assign(
		k, std::make_pair(
			is_saveable,
			v.value_or( "" )
		)
	);
	return it;
}

void ConfigManager::Save() {
	YAML::Node root;
	for ( const auto& kv : m_settings ) {
		if ( kv.second.first ) {
			root[ kv.first ] = kv.second.second;
		}
	}
	util::FS::WriteFile( m_config_path, Dump( root ) + util::String::EOLN() );
}

}
