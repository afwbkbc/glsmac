#pragma once

#include <stdexcept>

namespace world {

class Exception : public std::runtime_error {
public:
	Exception( const std::string& errmsg ); // TODO: file and line
};

}
