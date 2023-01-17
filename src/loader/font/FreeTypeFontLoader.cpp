#include "FreeTypeFontLoader.h"

using namespace std;

namespace loader {
namespace font {

FreeTypeFontLoader::~FreeTypeFontLoader() {
	for ( auto& it : m_fonts ) {
		DELETE( it.second );
	}
}

void FreeTypeFontLoader::Start() {
	if ( FT_Init_FreeType( &m_freetype ) )
		throw FontLoaderError( "Unable to initialize FreeType" );
}

void FreeTypeFontLoader::Stop() {
	FT_Done_FreeType( m_freetype );
}

void FreeTypeFontLoader::Iterate() {

}

types::Font *FreeTypeFontLoader::LoadFont( const string &name, const unsigned char size ) {
	
	string font_key = name + ":" + to_string(size);
	
	font_map_t::iterator it = m_fonts.find( font_key );
	if (it != m_fonts.end()) {
		return it->second;
	}
	else {
		Log( "Loading font \"" + font_key + "\"");

		NEWV( font, types::Font );
		font->m_name = name;

		FT_Face ftface;
		string file_name = GetRoot() + name;
		if ( FT_New_Face( m_freetype, file_name.c_str(), 0, &ftface ) ) {
			throw FontLoaderError( "Unable to load font \"" + name + "\"" );
		};
		FT_Set_Pixel_Sizes( ftface, 0, size );

		font->m_dimensions.width = font->m_dimensions.height = 0;

		FT_GlyphSlot g = ftface->glyph;
		types::Font::bitmap_t *bitmap;
		int sz;

		for (int i=32;i<128;i++) { // only ascii for now
			if ( FT_Load_Char( ftface, i, FT_LOAD_RENDER ) ) {
				throw FontLoaderError( "Font \"" + name + "\" bitmap loading failed" );
			};

			bitmap = &font->m_symbols[i];

			bitmap->ax = g->advance.x >> 6;
			bitmap->ay = g->advance.y >> 6;
			bitmap->width = g->bitmap.width;
			bitmap->height = g->bitmap.rows;
			bitmap->left = g->bitmap_left;
			bitmap->top = g->bitmap_top;
			sz = bitmap->width * bitmap->height;
			if ( sz > 0 ) {
				bitmap->data = (unsigned char*) malloc( sz );
				memcpy( ptr( bitmap->data, 0, sz ), g->bitmap.buffer, sz );
			}
			else {
				bitmap->data = nullptr;
			}

			font->m_dimensions.width += g->bitmap.width;
			font->m_dimensions.height = max( font->m_dimensions.height, (float)g->bitmap.rows );
		}

		FT_Done_Face( ftface );

		m_fonts[font_key] = font;
		
		DEBUG_STAT_INC( fonts_loaded );
		
		return font;
	}
}

} /* namespace font */
} /* namespace loader */
