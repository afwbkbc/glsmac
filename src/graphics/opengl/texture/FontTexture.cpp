#include <cmath>

#include "FontTexture.h"

#include "types/Vec22.h"
#include "engine/Engine.h"

static float tmpy = 0;

namespace graphics {
namespace opengl {

FontTexture::FontTexture( types::Font* font ) {
	m_name = font->m_name;

	//Log( "Creating OpenGL font texture" );

	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &m_texture );

	glBindTexture( GL_TEXTURE_2D, m_texture );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	ASSERT( !glGetError(), "Texture parameter error" );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, (GLsizei)font->m_dimensions.width, (GLsizei)font->m_dimensions.height, 0, GL_RED, GL_UNSIGNED_BYTE, 0 );
	ASSERT( !glGetError(), "Error loading image of font texture" );

	GLfloat ox = 0, oy = 0;
	types::Font::bitmap_t* bitmap;

	for ( int sym = 32 ; sym < 128 ; sym++ ) {

		bitmap = &font->m_symbols[ sym ];

		if ( bitmap->width > 0 && bitmap->height > 0 ) {
			ASSERT( bitmap->data, "Font bitmap data is null" );
			glTexSubImage2D( GL_TEXTURE_2D, 0, (GLint)ox, (GLint)oy, (GLsizei)bitmap->width, (GLsizei)bitmap->height, GL_RED, GL_UNSIGNED_BYTE, (const GLvoid*)ptr( bitmap->data, 0, bitmap->width * bitmap->height ) );
			ASSERT( !glGetError(), "Error loading subimage of font texture" );
		}

		m_tx[ sym ] = ox / font->m_dimensions.width;
		m_ty[ sym ] = oy / font->m_dimensions.height;

		ox += bitmap->width;
	};

	glBindTexture( GL_TEXTURE_2D, 0 );
}

FontTexture::~FontTexture() {
	//Log( "Destroying OpenGL font texture" );

	glDeleteTextures( 1, &m_texture );
}

} /* namespace opengl */
} /* namespace graphics */
