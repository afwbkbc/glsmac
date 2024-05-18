#pragma once

#include <string>

#include "loader/Loader.h"

namespace types {
class Font;
}

namespace loader {
namespace font {

CLASS( FontLoader, Loader )
	virtual types::Font* LoadFont( const std::string& name, const unsigned char size ) = 0;
};

}
}
