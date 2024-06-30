#pragma once

#include <string>

#include "FontLoader.h"

namespace loader {
namespace font {

CLASS( Null, FontLoader )
protected:
	types::Font* LoadFontImpl( const std::string& name, const std::string& path, const unsigned char size ) override { return nullptr; }
};

}
}
