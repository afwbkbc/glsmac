#pragma once

#include <string>

#include "FontLoader.h"

namespace loader {
namespace font {

CLASS( Null, FontLoader )
protected:
	types::Font* LoadFontImpl( const std::string& filename, const unsigned char size ) override { return nullptr; }
};

}
}
