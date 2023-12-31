#pragma once

#include <stdexcept>

namespace gse {

class Exception : public std::runtime_error {
public:

	Exception( const std::string& class_name, const std::string& reason )
		: std::runtime_error( "[" + class_name + "] " + reason )
		, class_name( class_name )
		, reason( reason ) {}

	const std::string class_name;
	const std::string reason;

};

}
