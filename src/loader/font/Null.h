#pragma once

#include <string>

#include "FontLoader.h"

namespace loader {
namespace font {

CLASS( Null, FontLoader )
protected:
	types::Font* LoadFontImpl( const std::string& name, const std::vector< unsigned char >& data, const unsigned char size ) override { return nullptr; }
};

}
}
