#include "Overlay.h"

namespace graphics {
namespace opengl {
namespace routine {

Overlay::Overlay( OpenGL* opengl, shader_program::Simple2D *shader_program )
	: Routine( opengl )
	, m_shader_program( shader_program )
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

	if ( m_is_redraw_needed ) {
		//Log( "Redrawing overlay" );
		m_fbo->WriteBegin();
		for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
			(*it)->Draw( m_shader_program );
			ASSERT( !glGetError(), "Overlay draw error" );
		}
		m_fbo->WriteEnd();
		m_is_redraw_needed = false;
	}
	
	glClear( GL_DEPTH_BUFFER_BIT ); // overlay must always be on top
	
	m_fbo->Draw( m_shader_program );
}

void Overlay::Redraw() {
	if ( !m_is_redraw_needed ) {
		m_is_redraw_needed = true;
	}
}

bool Overlay::SceneBelongs( const scene::Scene *scene ) const {
	return scene->GetType() == scene::SCENE_TYPE_SIMPLE2D;
}

} /* namespace routine */
} /* namespace opengl */
} /* namespace graphics */
