#include "FontTexture.h"

#include "graphics/opengl/OpenGL.h"

namespace graphics {
namespace opengl {

FontTexture::FontTexture( OpenGL* opengl, types::Font* font ) {
	m_name = font->m_name;

	//Log( "Creating OpenGL font texture" );

	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &m_texture );

	opengl->WithBindTexture(
		m_texture, [ this, &font ]() {

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

			ASSERT( !glGetError(), "Texture parameter error" );
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)font->m_dimensions.width, (GLsizei)font->m_dimensions.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
			glGenerateMipmap( GL_TEXTURE_2D );
			ASSERT( !glGetError(), "Error loading image of font texture" );

			GLfloat ox = 0, oy = 0;
			types::Font::bitmap_t* bitmap;

			for ( int sym = 32 ; sym < 128 ; sym++ ) {

				bitmap = &font->m_symbols[ sym ];

				if ( bitmap->width > 0 && bitmap->height > 0 ) {
					ASSERT( bitmap->data, "Font bitmap data is null" );

					// convert to RGBA
					const size_t sz = bitmap->width * bitmap->height;
					unsigned char* data = (unsigned char*)malloc( sz * 4 );
					memset( ptr( data, 0, sz * 4 ), 0xff, sz * 4 );
					for ( auto i = 0 ; i < sz ; i++ ) {
						data[ i * 4 + 3 ] = bitmap->data[ i ];
					}

					glTexSubImage2D( GL_TEXTURE_2D, 0, (GLint)ox, (GLint)oy, (GLsizei)bitmap->width, (GLsizei)bitmap->height, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)data );

					free( data );

					ASSERT( !glGetError(), "Error loading subimage of font texture" );
				}

				m_tx[ sym ] = ox / font->m_dimensions.width;
				m_ty[ sym ] = oy / font->m_dimensions.height;

				ox += bitmap->width;
			};

		}
	);
}

FontTexture::~FontTexture() {
	//Log( "Destroying OpenGL font texture" );

	glDeleteTextures( 1, &m_texture );
}

}
}
