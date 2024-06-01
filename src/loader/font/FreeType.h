#pragma once

#include <unordered_map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "FontLoader.h"

namespace loader {
namespace font {

CLASS( FreeType, FontLoader )

	~FreeType();

	void Start() override;
	void Stop() override;
	void Iterate() override;

protected:

	types::Font* LoadFontImpl( const std::string& filename, const unsigned char size ) override;

private:
	FT_Library m_freetype;

	// cache all fonts for future use
	typedef std::unordered_map< std::string, types::Font* > font_map_t;
	font_map_t m_fonts;
};

}
}
