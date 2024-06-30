#pragma once

#include <string>

#include "Object.h"

#include "gse/Exception.h"

namespace gse {
namespace type {

class Exception : public Object {
public:

	Exception( const gse::Exception& exception, const std::vector< std::string >& backtrace );

};

}
}
