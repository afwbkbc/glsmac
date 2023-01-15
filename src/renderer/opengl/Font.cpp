#include <cmath>

#include "Font.h"

#include "types/Vec22.h"
#include "engine/Engine.h"

static float tmpy = 0;

namespace renderer {
namespace opengl {

Font::Font( types::Font *font, shader_program::FontOpenGLShaderProgram *shader_program ) : m_font( font ), m_shader_program( shader_program ) {
	m_name = font->m_name;

	Log( "Creating OpenGL font" );

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

	//memset( m_boxes_count, 0, sizeof( m_boxes_count ) );
		
	for ( int sym = 32; sym < 128; sym++ ) { // only ascii for now

		bitmap = &m_font->m_symbols[sym];

		if ( bitmap->data ) {
			glTexSubImage2D( GL_TEXTURE_2D, 0, (GLint)ox, (GLint)oy, (GLsizei)bitmap->width, (GLsizei)bitmap->height, GL_RED, GL_UNSIGNED_BYTE, (const GLvoid *)bitmap->data );
			if ( glGetError() ) {
				throw FontError( "Error loading subimage of font texture" );
			};
		}
		else {
			throw FontError( "font bitmap data is null" );
		}

		//glGenBuffers( 1, &m_vbos[sym]);

		m_tx[sym] = ox / m_font->m_dimensions.width;
		m_ty[sym] = oy / m_font->m_dimensions.height;

		ox += bitmap->width;
	};

	glBindTexture( GL_TEXTURE_2D, 0 );
	
}

Font::~Font() {
	Log( "Destroying OpenGL font" );

	glDeleteTextures(1, &m_texture );
}

void Font::Update( font_context_t* ctx, const string& text, const float x, const float y ) {
	if ( text != ctx->m_text || x != ctx->m_x || y != ctx->m_y ) {
		
		Log( "Setting text to " + text );
		
		types::Font::bitmap_t *bitmap;
		
		const float sx = 2.0 / g_engine->GetRenderer()->GetWindowWidth();
		const float sy = 2.0 / g_engine->GetRenderer()->GetWindowHeight();	

		vector<vertex_box_t> boxes[128] = {};
		
		for ( int sym = 32; sym < 128; sym++ ) { // only ascii for now
			boxes[sym].clear();
		}
		
		float cx = x;
		float cy = y;
		
		for ( const char *p = text.c_str(); *p; p++ ) {
			unsigned char sym = (unsigned char)*p;
			
#if DEBUG
			if ( sym < 32 || sym >= 128 ) {
				throw FontError( "unexpected font character " + to_string( sym ) );
			}
#endif
			
			bitmap = &m_font->m_symbols[sym];
			
			float x2 = cx + bitmap->left * sx;
			float y2 = -cy - bitmap->top * sy;
			float w = bitmap->width * sx;
			float h = bitmap->height * sy;

			float tbx1 = this->m_tx[sym];
			float tby1 = this->m_ty[sym];
			float tbx2 = this->m_tx[sym] + bitmap->width / m_font->m_dimensions.width;
			float tby2 = this->m_ty[sym] + bitmap->height / m_font->m_dimensions.height;
			
			boxes[sym].push_back({
				{x2,     -y2    , tbx1, tby1},
				{x2 + w, -y2    , tbx2, tby1},
				{x2,     -y2 - h, tbx1, tby2},
				{x2 + w, -y2 - h, tbx2, tby2},
			});
			
			cx += bitmap->ax * sx;
			cy += bitmap->ay * sy;
		}

		for ( int sym = 32; sym < 128; sym++ ) { // only ascii for now
			ctx->m_boxes_count[sym] = boxes[sym].size();
			
			glBindBuffer( GL_ARRAY_BUFFER, ctx->m_vbos[sym] );
			if ( ctx->m_boxes_count[sym] > 0 ) {
				glBufferData( GL_ARRAY_BUFFER, sizeof(vertex_box_t) * ctx->m_boxes_count[sym], boxes[sym].data(), GL_DYNAMIC_DRAW );
			}
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}
		
		ctx->m_text = text;
		ctx->m_x = x;
		ctx->m_y = y;
	}
}

void Font::Render( const font_context_t* ctx, float z_index, types::Color color ) {

	if ( !ctx->m_text.empty() ) {
	
		for ( const char *p = ctx->m_text.c_str(); *p; p++ ) {
			unsigned char sym = (unsigned char)*p;

			glBindBuffer( GL_ARRAY_BUFFER, ctx->m_vbos[sym] );

			glActiveTexture( GL_TEXTURE0 );
			glBindTexture( GL_TEXTURE_2D, m_texture );

			m_shader_program->Enable();

			glUniform1i( m_shader_program->m_gl_uniforms.texture, 0 );
			glUniform4fv( m_shader_program->m_gl_uniforms.color, 1, (const GLfloat *)&color );
			glUniform1f( m_shader_program->m_gl_uniforms.z_index, z_index );

			for ( size_t c = 0 ; c < ctx->m_boxes_count[sym] ; c++ ) {
				glDrawArrays( GL_TRIANGLE_STRIP, c * 4, 4 );
			}

			m_shader_program->Disable();

			glBindTexture( GL_TEXTURE_2D, 0 );
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
};

} /* namespace opengl */
} /* namespace renderer */
