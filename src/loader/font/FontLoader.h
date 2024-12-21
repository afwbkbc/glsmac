#pragma once

#include <string>
#include <unordered_map>

#include "loader/Loader.h"

#include "resource/Types.h"
#include "types/Vec2.h"

namespace types {
class Font;
}

namespace loader {
namespace font {

CLASS( FontLoader, Loader )
	virtual ~FontLoader();

	types::Font* LoadFont( const resource::resource_t res, const unsigned char size );
	types::Font* GetBuiltinFont( const unsigned char size );

protected:
	virtual types::Font* LoadFontImpl( const std::string& name, const std::vector< unsigned char >& data, const unsigned char size ) = 0;

private:
	// cache all fonts for future use
	std::unordered_map< std::string, types::Font* > m_fonts;
	std::unordered_map< unsigned char, types::Font* > m_builtin_fonts;
};

}
}
