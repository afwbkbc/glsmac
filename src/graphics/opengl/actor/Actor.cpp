#include "Actor.h"

#include "graphics/opengl/OpenGL.h"
#include "graphics/opengl/FBO.h"
#include "graphics/opengl/shader_program/ShaderProgram.h"
#include "scene/actor/Actor.h"

namespace graphics {
namespace opengl {

Actor::Actor( OpenGL* opengl, scene::actor::Actor* actor )
	: m_opengl( opengl )
	, m_actor( actor ) {
	m_name = actor->GetLocalName();
	m_fbo = m_opengl->CreateFBO();
}

Actor::~Actor() {
	m_opengl->DestroyFBO( m_fbo );
}

void Actor::Draw( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {
	switch ( shader_program->GetType() ) {
		case shader_program::ShaderProgram::TYPE_SIMPLE2D: {
			if ( m_is_redraw_needed ) {
				//Log( "Redrawing overlay" );
				//glDisable( GL_DEPTH_TEST );
				m_fbo->Write(
					[ this, &shader_program, &camera ] {
						DrawImpl( shader_program, camera );
					}
				);
				//glEnable( GL_DEPTH_TEST );
				//m_is_redraw_needed = false;
			}
			m_fbo->Draw( shader_program );
			break;
		}
		default: {
			DrawImpl( shader_program, camera );
		}
	}
}

const float Actor::GetZIndex() const {
	return m_z_index;
}

void Actor::SetZIndex( const float z_index ) {
	m_z_index = z_index;
}

}
}
