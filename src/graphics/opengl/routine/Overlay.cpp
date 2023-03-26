#include "Overlay.h"

#include "../actor/Text.h"

namespace graphics {
namespace opengl {
namespace routine {

Overlay::Overlay( OpenGL* opengl, shader_program::Simple2D *shader_program, shader_program::Font *shader_program_font )
	: Routine( opengl )
	, m_shader_program( shader_program )
	, m_shader_program_font( shader_program_font )
{
	//
}

void Overlay::Start() {
	m_fbo = m_opengl->CreateFBO();
}

void Overlay::Stop() {
	m_opengl->DestroyFBO( m_fbo );
}

void Overlay::Iterate() {

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it )
		(*it)->Update();

	glClear( GL_DEPTH_BUFFER_BIT ); // overlay must always be on top
	
	if ( m_is_redraw_needed ) {
		
		glDisable( GL_DEPTH_TEST );
		
		//Log( "Redrawing overlay" );
		m_fbo->WriteBegin();
		for ( auto& scene : m_gl_scenes ) {
			switch ( scene->GetScene()->GetType() ) {
				case scene::SCENE_TYPE_SIMPLE2D: {
					scene->Draw( m_shader_program );
					break;
				}
				case scene::SCENE_TYPE_TEXT: {
					scene->Draw( m_shader_program_font );
					break;
				}
				default: {
					ASSERT( false, "unknown scene type " + std::to_string( scene->GetScene()->GetType() ) );
				}
			}
			ASSERT( !glGetError(), "Overlay draw error" );
		}
		m_fbo->WriteEnd();
		
		glEnable( GL_DEPTH_TEST );
	
		m_is_redraw_needed = false;
	}
	
	m_fbo->Draw( m_shader_program );
	
}

opengl::Actor *Overlay::AddCustomActor( scene::actor::Actor *actor ) {
	auto actor_type = actor->GetType();
	switch (actor_type) {
		case scene::actor::Actor::TYPE_TEXT: {
			auto *text_actor = (scene::actor::Text *)actor;
			auto *font = text_actor->GetFont();
			//base::ObjectLink *font_link = font->m_graphics_object;
			//Font *gl_font;
			//if ( font_link != NULL )
				//gl_font = font_link->GetDstObject<Font>();
			//else {
				//m_shader_program->Enable();
				//NEW( gl_font, Font, font, m_shader_program );
				//m_shader_program->Disable();
				//NEW( font_link, base::ObjectLink, font, gl_font );
				//m_gl_fonts.push_back( font_link );
				//font->m_graphics_object = font_link;
			//}
			NEWV( result, Text, (scene::actor::Text *)actor, font );
			return result;
		}
		default: {
			ASSERT( false, "unknown actor type " + std::to_string( actor_type ) );
		}
	}
	return NULL;
}

void Overlay::Redraw() {
	if ( !m_is_redraw_needed ) {
		m_is_redraw_needed = true;
	}
}

bool Overlay::SceneBelongs( const scene::Scene *scene ) const {
	return
		scene->GetType() == scene::SCENE_TYPE_SIMPLE2D ||
		scene->GetType() == scene::SCENE_TYPE_TEXT
	;
}

} /* namespace routine */
} /* namespace opengl */
} /* namespace graphics */
