#pragma once

#include <string>
#include <functional>
#include <map>
#include <optional>

#include "Util.h"

namespace util {

#define AH( ... ) [ __VA_ARGS__ ]( const std::string& value ) -> void

CLASS( ConfigParser, Util )

	ConfigParser( const std::string& path );

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

	void ParseArgs( const int argc, const char* argv[] );
	void ParseFile( const std::string& path );

private:

	const std::string m_path;

	std::map< std::string, arg_rule_t > m_arg_rules;
	const size_t m_offset_step = 8;
	size_t m_offset = 0;

	const std::map< std::string, arg_rule_t >::iterator Set( const std::string& k, const std::optional< std::string >& v, const std::string& err_prefix );
};

}
