#include "TextActor.h"

#include "shader_program/FontOpenGLShaderProgram.h"
#include "engine/Engine.h"

namespace renderer {
namespace opengl {

using namespace shader_program;

TextActor::TextActor( scene::actor::TextActor *actor, types::Font *font ) : Actor( actor ), m_font( font ) {
	Log( "Creating OpenGL text '" + actor->GetText() + "' with " + m_font->GetName() + " for " + actor->GetName() );
	NEW( m_texture, FontTexture, m_font );
	glGenBuffers( 1, &m_vbo );
	auto *text_actor = (const scene::actor::TextActor *)m_actor;
	auto position = m_actor->GetPosition();
	Update( text_actor->GetText(), position.x, position.y );
}

TextActor::~TextActor() {
	Log( "Destroying OpenGL text" );
	glDeleteBuffers( 1, &m_vbo );
	DELETE( m_texture );
}

void TextActor::Update( const string& text, const float x, const float y ) {
	
	const float sx = 2.0 / g_engine->GetRenderer()->GetWindowWidth();
	const float sy = 2.0 / g_engine->GetRenderer()->GetWindowHeight();	
	
	vector<vertex_box_t> boxes = {};
	
	for ( int sym = 32; sym < 128; sym++ ) {
		boxes.clear();
	}
	
	float cx = x;
	float cy = y;
	
	types::Font::bitmap_t *bitmap;
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
		
		float tbx1 = m_texture->m_tx[sym];
		float tby1 = m_texture->m_ty[sym];
		float tbx2 = m_texture->m_tx[sym] + bitmap->width / m_font->m_dimensions.width;
		float tby2 = m_texture->m_ty[sym] + bitmap->height / m_font->m_dimensions.height;
		
		boxes.push_back({
			{x2,     -y2    , tbx1, tby1},
			{x2 + w, -y2    , tbx2, tby1},
			{x2,     -y2 - h, tbx1, tby2},
			{x2 + w, -y2 - h, tbx2, tby2},
		});
		
		cx += bitmap->ax * sx;
		cy += bitmap->ay * sy;
	}
	
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	m_boxes_count = boxes.size();
	
	if ( !boxes.empty() ) {
		glBufferData( GL_ARRAY_BUFFER, sizeof(vertex_box_t) * boxes.size(), boxes.data(), GL_STATIC_DRAW );
	}
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
}

void TextActor::Draw( OpenGLShaderProgram *shader_program ) {
	if ( m_boxes_count > 0 ) {
		auto *sp = (FontOpenGLShaderProgram*)shader_program;
		
		auto *text_actor = (const scene::actor::TextActor *)m_actor;
		auto position = m_actor->GetPosition();
	
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
		
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, m_texture->m_texture );
		
		sp->Enable();
		glUniform1i( sp->m_gl_uniforms.texture, 0 );
		glUniform4fv( sp->m_gl_uniforms.color, 1, (const GLfloat *)&text_actor->GetColor() );
		glUniform1f( sp->m_gl_uniforms.z_index, position.z );
		
		for ( size_t c = 0 ; c < m_boxes_count ; c++ ) {
			glDrawArrays( GL_TRIANGLE_STRIP, c * 4, 4 );
		}
		
		sp->Disable();
		
		glBindTexture( GL_TEXTURE_2D, 0 );
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
}

} /* namespace opengl */
} /* namespace renderer */
