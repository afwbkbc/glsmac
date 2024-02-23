#pragma once

#include <stdexcept>
#include <string>

namespace types {

class Exception : public std::runtime_error {
public:
	Exception( const std::string& class_name, const std::string& reason )
		: std::runtime_error( "[" + class_name + "] " + reason ) {}
};

}
