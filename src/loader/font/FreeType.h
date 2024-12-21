#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "FontLoader.h"

namespace loader {
namespace font {

CLASS( FreeType, FontLoader )

	void Start() override;
	void Stop() override;

protected:

	types::Font* LoadFontImpl( const std::string& name, const std::vector< unsigned char >& data, const unsigned char size ) override;

private:
	FT_Library m_freetype;

};

}
}
