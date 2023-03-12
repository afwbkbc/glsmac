#pragma once

#include <string>
#include <functional>
#include <map>

#include "Util.h"

namespace util {

#define AH( ... ) [ __VA_ARGS__ ]( const std::string& value ) -> void

CLASS( ArgParser, Util )
	
	ArgParser( const int argc, const char *argv[] );
	
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
	void Parse();
	
private:
	const int m_argc = 0;
	const char** const m_argv = nullptr;
	
	std::map< std::string,  arg_rule_t > m_arg_rules;
	const size_t m_offset_step = 8;
	size_t m_offset = 0;
	
};

}
