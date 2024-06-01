#pragma once

#include <string>

#include "loader/Loader.h"

#include "resource/Types.h"

namespace types {
class Font;
}

namespace loader {
namespace font {

CLASS( FontLoader, Loader )
	types::Font* LoadFont( const resource::resource_t res, const unsigned char size );
protected:
	virtual types::Font* LoadFontImpl( const std::string& filename, const unsigned char size ) = 0;
};

}
}
