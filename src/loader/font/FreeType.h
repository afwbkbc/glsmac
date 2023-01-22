#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <unordered_map>

#include "FontLoader.h"

namespace loader {
namespace font {

CLASS( FreeType, FontLoader )
	
	~FreeType();
	
	void Start();
	void Stop();
	void Iterate();

	types::Font *LoadFont( const std::string &name, const unsigned char size );
	
protected:
	FT_Library m_freetype;
	
	// cache all fonts for future use
	typedef unordered_map< string, types::Font* > font_map_t;
	font_map_t m_fonts;
};

} /* namespace font */
} /* namespace loader */
