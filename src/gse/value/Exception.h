#pragma once

#include <string>

#include "Object.h"

#include "gse/Exception.h"

namespace gse {
namespace value {

class Exception : public Object {
public:

	Exception( GSE_CALLABLE, const gse::Exception& exception, const std::vector< std::string >& stacktrace );

#if defined( DEBUG ) || defined( FASTDEBUG )
	const std::string ToString() override;
#endif
};

}
}
