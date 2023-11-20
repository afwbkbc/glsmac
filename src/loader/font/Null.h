#pragma once

#include <string>

#include "FontLoader.h"

namespace loader {
namespace font {

CLASS( Null, FontLoader )
	types::Font* LoadFont( const std::string& name, const unsigned char size ) override { return nullptr; }
};

}
}
