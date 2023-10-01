#include "Text.h"

#include "engine/Engine.h"

using namespace types;

namespace graphics {
namespace opengl {

Text::Text( scene::actor::Text* actor, Font* font )
	: Actor( actor )
	, m_font( font ) {
	//Log( "Creating OpenGL text '" + actor->GetText() + "' with font " + font->m_name );
	glGenBuffers( 1, &m_vbo );
	auto* text_actor = (const scene::actor::Text*)m_actor;
	auto position = m_actor->GetPosition();
	Update( m_font, text_actor->GetText(), position.x, position.y );
}

Text::~Text() {
	//Log( "Destroying OpenGL text" );
	glDeleteBuffers( 1, &m_vbo );
	if ( m_texture ) {
		DELETE( m_texture );
	}
}

void Text::Update( Font* font, const std::string& text, const float x, const float y ) {

	const auto* g = g_engine->GetGraphics();
	const Vec2< size_t > window_size = {
		g->GetViewportWidth(),
		g->GetViewportHeight()
	};

	const bool need_reload = ( font != m_font || text != m_text || window_size != m_last_window_size );

	if ( need_reload ) {

		if ( m_font != font ) {
			//Log( "Changing font from " + m_font->m_name + " to " + font->m_name );
			m_font = font;
		}
		if ( m_text != text ) {
			//Log( "Changing text from " + m_text + " to " + text );
			m_text = text;
		}

		if ( m_last_window_size != window_size ) {
			//Log( "Resizing for window size " + window_size.ToString() );
			m_last_window_size = window_size;
		}

		if ( m_font ) {

			if ( !m_texture ) {
				NEW( m_texture, FontTexture, m_font );
			}

			const float sx = 2.0 / g_engine->GetGraphics()->GetViewportWidth();
			const float sy = 2.0 / g_engine->GetGraphics()->GetViewportHeight();

			std::vector< vertex_box_t > boxes = {};

			for ( int sym = 32 ; sym < 128 ; sym++ ) {
				boxes.clear();
			}

			float cx = 0;
			float cy = 0;

			Font::bitmap_t* bitmap = nullptr;
			for ( const char* p = m_text.c_str() ; *p ; p++ ) {
				unsigned char sym = (unsigned char)*p;

				//ASSERT( sym >= 32 && sym < 128, "unexpected font character " + std::to_string( sym ) );
				if ( sym < 32 || sym >= 128 ) {
					sym = ' '; // replace unprintable characters with spaces
				}

				bitmap = &m_font->m_symbols[ sym ];

				float x2 = cx + bitmap->left * sx;
				float y2 = -cy - bitmap->top * sy;
				float w = bitmap->width * sx;
				float h = bitmap->height * sy;

				float tbx1 = m_texture->m_tx[ sym ];
				float tby1 = m_texture->m_ty[ sym ];
				float tbx2 = m_texture->m_tx[ sym ] + bitmap->width / m_font->m_dimensions.width;
				float tby2 = m_texture->m_ty[ sym ] + bitmap->height / m_font->m_dimensions.height;

				boxes.push_back(
					{
						{ x2,     -y2,     tbx1, tby1 },
						{ x2 + w, -y2,     tbx2, tby1 },
						{ x2,     -y2 - h, tbx1, tby2 },
						{ x2 + w, -y2 - h, tbx2, tby2 },
					}
				);

				cx += bitmap->ax * sx;
				cy += bitmap->ay * sy;
			}

			glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
			m_boxes_count = boxes.size();

			if ( !boxes.empty() ) {
				glBufferData( GL_ARRAY_BUFFER, sizeof( vertex_box_t ) * boxes.size(), boxes.data(), GL_STATIC_DRAW );
			}

			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}
	}

	if ( m_coords.x != x || m_coords.y != y ) {
		//Log( "Setting coordinates to " + std::to_string( x ) + "x" + std::to_string( y ) );
		m_coords = {
			x,
			y
		};
	}
}

void Text::Draw( shader_program::ShaderProgram* shader_program, Camera* camera ) {
	if ( m_boxes_count > 0 ) {
		auto* sp = (shader_program::Font*)shader_program;

		auto* text_actor = (const scene::actor::Text*)m_actor;

		glBindBuffer( GL_ARRAY_BUFFER, m_vbo );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, m_texture->m_texture );

		sp->Enable();

		auto flags = text_actor->GetRenderFlags();

		glUniform1ui( sp->uniforms.flags, flags );
		if ( flags & actor::Actor::RF_USE_AREA_LIMITS ) {
			const auto& limits = text_actor->GetAreaLimits();
			glUniform3fv( sp->uniforms.area_limits.min, 1, (const GLfloat*)&limits.first );
			glUniform3fv( sp->uniforms.area_limits.max, 1, (const GLfloat*)&limits.second );
		}

		if ( flags & actor::Actor::RF_USE_2D_POSITION ) {
			glUniform2fv( sp->uniforms.position, 1, (const GLfloat*)&m_coords );
		}

		glUniform1i( sp->uniforms.texture, 0 );
		glUniform4fv( sp->uniforms.color, 1, (const GLfloat*)&text_actor->GetColor() );
		auto position = m_actor->GetPosition();
		glUniform1f( sp->uniforms.z_index, position.z );

		for ( size_t c = 0 ; c < m_boxes_count ; c++ ) {
			glDrawArrays( GL_TRIANGLE_STRIP, c * 4, 4 );
		}

		sp->Disable();

		glBindTexture( GL_TEXTURE_2D, 0 );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}

}

} /* namespace opengl */
} /* namespace graphics */
