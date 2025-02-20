#pragma once

#include <string>

#include "Object.h"

#include "gse/Exception.h"

namespace gse {
namespace value {

class Exception : public Object {
public:

	Exception( gc::Space* const gc_space, const gse::Exception& exception, const std::vector< std::string >& stacktrace );

};

}
}
