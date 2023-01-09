#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "FontLoader.h"

namespace loader {
namespace font {

MINOR_CLASS( FreeType, FontLoader )
	FreeTypeFontLoader( const std::string & fonts_directory ) : m_fonts_directory( fonts_directory ) { } ;
	void Start();
	void Stop();
	void Iterate();

	types::Font *LoadFont( const std::string &name, const unsigned char size );
protected:
	const std::string m_fonts_directory;

	FT_Library m_freetype;
};

} /* namespace font */
} /* namespace loader */
