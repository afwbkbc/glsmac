#pragma once

#include <string>
#include <functional>
#include <map>
#include <optional>

#include "Util.h"

namespace config {
class Config;
}

namespace util {

#define AH( ... ) [ __VA_ARGS__ ]( const std::string& value ) -> void

CLASS( ConfigManager, Util )

	ConfigManager( const std::string& path, const std::string& config_path );

	typedef std::function< void( const std::string& value ) > arg_handler_t;

	struct arg_rule_t {
		std::string arg;
		bool has_value;
		std::string value_name;
		std::string description;
		arg_handler_t handler;
	};

	void AddRule( const std::string& argument, const std::string& description, const arg_handler_t handler );
	void AddRule( const std::string& argument, const std::string& value_name, const std::string& description, const arg_handler_t handler );
	const std::string GetHelpString() const;
	const std::string GetUnknownArgumentNote() const;

	void ParseFile( const std::string& path = "" );
	void ParseArgs( const int argc, const char* argv[] );

	void UpdateSetting( const std::string& key, const std::string& value );

private:
	friend class config::Config;
	typedef std::map< std::string, std::pair< bool, std::string > > settings_t;
	const settings_t& GetSettings() const;

private:

	settings_t m_settings = {};

	const std::string m_path;
	const std::string m_config_path;

	std::map< std::string, arg_rule_t > m_arg_rules;
	const size_t m_offset_step = 8;
	size_t m_offset = 0;

	const std::map< std::string, arg_rule_t >::iterator Set( const std::string& k, const std::optional< std::string >& v, const std::string& err_prefix, const bool is_saveable );

	void Save();
};

}
