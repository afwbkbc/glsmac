#include "FontLoader.h"

namespace loader {
namespace font {

types::Font* FontLoader::LoadFont( const resource::resource_t res, const unsigned char size ) {
	return LoadFontImpl( GetFilename( res ), size );
}

}
}
