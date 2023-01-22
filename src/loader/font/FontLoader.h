#pragma once

#include <string>

#include "../Loader.h"

#include "types/Font.h"

namespace loader {
namespace font {

CLASS( FontLoader, Loader )
	virtual types::Font *LoadFont( const std::string &name, const unsigned char size ) = 0;
};

} /* namespace font */
} /* namespace loader */
