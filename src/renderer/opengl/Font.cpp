#include <cmath>

#include "Font.h"

#include "types/Vec22.h"

namespace renderer {
namespace opengl {

Font::Font( types::Font *font, shader_program::FontOpenGLShaderProgram *shader_program ) : m_font( font ), m_shader_program( shader_program ) {
	m_name = font->m_name;

	Log( "Creating OpenGL font" );

	glGenBuffers( 1, &m_vbo);
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );

	glActiveTexture( GL_TEXTURE0 );
	glGenTextures( 1, &m_texture );

	glBindTexture( GL_TEXTURE_2D, m_texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	if ( glGetError() ) {
		throw FontError( "Texture uniform error" );
	}
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, (GLsizei)m_font->m_dimensions.width, (GLsizei)m_font->m_dimensions.height, 0, GL_RED, GL_UNSIGNED_BYTE, 0 );
	if ( glGetError() ) {
		throw FontError( "Error loading image of font texture" );
	};

	GLfloat ox = 0,oy = 0;
	types::Font::bitmap_t *bitmap;
	for( int ii = 32; ii < 128; ii++ ) { // only ascii for now

		bitmap = &m_font->m_symbols[ii];

		if ( bitmap->data ) {

			glTexSubImage2D( GL_TEXTURE_2D, 0, (GLint)ox, (GLint)oy, (GLsizei)bitmap->width, (GLsizei)bitmap->height, GL_RED, GL_UNSIGNED_BYTE, (const GLvoid *)bitmap->data );
			if ( glGetError() ) {
				throw FontError( "Error loading subimage of font texture" );
			};
		}

		m_tx[ii] = ox / m_font->m_dimensions.width;
		m_ty[ii] = oy / m_font->m_dimensions.height;

		ox += bitmap->width;
	};

	//glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

Font::~Font() {
	Log( "Destroying OpenGL font" );

	glDeleteBuffers( 1, &m_vbo );
	glDeleteTextures(1, &m_texture );
}

void Font::Render( const char *text, float x, float y, float z_index, types::Color color ) const {
	const float sx = 2.0 / m_shader_program->GetWindowWidth();
	const float sy = 2.0 / m_shader_program->GetWindowHeight();

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );

	m_shader_program->Enable();

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_texture );
	glUniform1i( m_shader_program->m_gl_uniforms.texture, 0 );
	glUniform4fv( m_shader_program->m_gl_uniforms.color, 1, (const GLfloat *)&color );
	glUniform1f( m_shader_program->m_gl_uniforms.z_index, z_index );

	for ( const char *p = text; *p; p++ ) {
		unsigned char sym = (unsigned char)*p;
		types::Font::bitmap_t *bitmap = &m_font->m_symbols[sym];

		float x2 = x + bitmap->left * sx;
		float y2 = -y - bitmap->top * sy;
		float w = bitmap->width * sx;
		float h = bitmap->height * sy;

		float tbx1 = this->m_tx[sym];
		float tby1 = this->m_ty[sym];
		float tbx2 = this->m_tx[sym] + bitmap->width / m_font->m_dimensions.width;
		float tby2 = this->m_ty[sym] + bitmap->height / m_font->m_dimensions.height;

		GLfloat box[4][4] = {
			{x2,     -y2    , tbx1, tby1},
			{x2 + w, -y2    , tbx2, tby1},
			{x2,     -y2 - h, tbx1, tby2},
			{x2 + w, -y2 - h, tbx2, tby2},
		};

	    glBufferData( GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW );
	    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

	    x += bitmap->ax * sx;
	    y += bitmap->ay * sy;
	}

	m_shader_program->Disable();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
};

} /* namespace opengl */
} /* namespace renderer */
