#include "FreeType.h"

#include "types/Font.h"

namespace loader {
namespace font {

void FreeType::Start() {
	auto res = FT_Init_FreeType( &m_freetype );
	if ( res ) {
		THROW( "Unable to initialize FreeType: " + std::to_string( res ) );
	}
}

void FreeType::Stop() {
	FT_Done_FreeType( m_freetype );
}

types::Font* FreeType::LoadFontImpl( const std::string& name, const std::vector< unsigned char >& data, const unsigned char size ) {

	const auto font_key = name + ":" + std::to_string( size );

	int res = 0;

	Log( "Loading font \"" + font_key + "\"" );

	NEWV( font, types::Font, font_key );

	FT_Face ftface;
	res = FT_New_Memory_Face( m_freetype, data.data(), data.size(), 0, &ftface );
	if ( res ) {
		THROW( "Unable to load font \"" + name + "\": " + std::to_string( res ) );
	}

	FT_Set_Pixel_Sizes( ftface, 0, size );

	font->m_dimensions.width = font->m_dimensions.height = 0;

	FT_GlyphSlot g = ftface->glyph;
	types::Font::bitmap_t* bitmap;
	int sz;

	for ( int i = 32 ; i < 128 ; i++ ) { // only ascii for now
		res = FT_Load_Char( ftface, i, FT_LOAD_RENDER );
		if ( res ) {
			THROW( "Font \"" + name + "\" bitmap loading failed: " + std::to_string( res ) );
		}

		bitmap = &font->m_symbols[ i ];

		bitmap->ax = g->advance.x >> 6;
		bitmap->ay = g->advance.y >> 6;
		bitmap->width = g->bitmap.width;
		bitmap->height = g->bitmap.rows;
		bitmap->left = g->bitmap_left;
		bitmap->top = g->bitmap_top;
		sz = bitmap->width * bitmap->height;
		if ( sz > 0 ) {
			bitmap->data = (unsigned char*)malloc( sz );
			memcpy( ptr( bitmap->data, 0, sz ), g->bitmap.buffer, sz );
		}
		else {
			bitmap->data = nullptr;
		}

		font->m_dimensions.width += g->bitmap.width;
		font->m_dimensions.height = std::max( font->m_dimensions.height, (float)g->bitmap.rows );
	}

	FT_Done_Face( ftface );

	DEBUG_STAT_INC( fonts_loaded );

	return font;
}

}
}
