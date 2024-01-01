#pragma once

#include <stdexcept>
#include <vector>

namespace gse {

class Exception : public std::runtime_error {
public:

	Exception( const std::string& class_name, const std::string& reason, const std::vector< std::string >& backtrace )
		: std::runtime_error( "[" + class_name + "] " + reason )
		, class_name( class_name )
		, reason( reason )
		, backtrace( backtrace ) {}

	const std::string class_name;
	const std::string reason;
	const std::vector< std::string > backtrace;

};

}
