#include "Overlay.h"


#include "graphics/opengl/OpenGL.h"
#include "graphics/opengl/FBO.h"
#include "graphics/opengl/actor/Text.h"
#include "graphics/opengl/shader_program/Simple2D.h"
#include "scene/Scene.h"
#include "scene/actor/Actor.h"
#include "scene/actor/Text.h"

namespace graphics {
namespace opengl {
namespace routine {

Overlay::Overlay( OpenGL* opengl, shader_program::Simple2D* shader_program )
	: Routine( opengl )
	, m_shader_program( shader_program ) {
	//
}

void Overlay::Start() {
	m_fbo = m_opengl->CreateFBO();
}

void Overlay::Stop() {
	m_opengl->DestroyFBO( m_fbo );
}

void Overlay::Iterate() {

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
		if ( *it ) {
			( *it )->Update();
		}
	}
	// Safety check: ensure this pointer is valid before accessing members
	bool is_redraw_needed = false;
	try {
		is_redraw_needed = m_is_redraw_needed;
	} catch (...) {
		return; // Exit early if object is corrupted
	}
	if ( is_redraw_needed ) {
		if ( !m_fbo ) {
			THROW( "FBO not initialized in Overlay::Iterate()" );
		}
		if ( !m_shader_program ) {
			THROW( "Shader program not initialized in Overlay::Iterate()" );
		}

		glDisable( GL_DEPTH_TEST );

		//Log( "Redrawing overlay" );
		m_fbo->Write(
			[ this ]() {
				if ( !m_shader_program ) {
					THROW( "Shader program null in Overlay Write lambda" );
				}
				for ( auto& scene : m_gl_scenes ) {
					if ( !scene ) {
						THROW( "Scene null in Overlay Write lambda" );
					}
					if ( !scene->GetScene() ) {
						THROW( "Scene->GetScene() null in Overlay Write lambda" );
					}
					switch ( scene->GetScene()->GetType() ) {
						case scene::SCENE_TYPE_SIMPLE2D: {
							scene->Draw( m_shader_program );
							break;
						}
						default: {
							THROW( "unknown scene type " + std::to_string( scene->GetScene()->GetType() ) );
						}
					}
					ASSERT( !glGetError(), "Overlay draw error" );
				}
			}
		);

		glEnable( GL_DEPTH_TEST );

		m_is_redraw_needed = false;
	}

	if ( !m_fbo ) {
		THROW( "FBO not initialized for Draw() in Overlay::Iterate()" );
	}
	if ( !m_shader_program ) {
		THROW( "Shader program not initialized for Draw() in Overlay::Iterate()" );
	}
	m_fbo->Draw( m_shader_program );
}

opengl::Actor* Overlay::AddCustomActor( scene::actor::Actor* actor ) {
	auto actor_type = actor->GetType();
	switch ( actor_type ) {
		case scene::actor::Actor::TYPE_TEXT: {
			auto* text_actor = (scene::actor::Text*)actor;
			NEWV( result, Text, m_opengl, text_actor, text_actor->GetFont() );
			return result;
		}
		default: {
			THROW( "unknown actor type " + std::to_string( actor_type ) );
		}
	}
}

void Overlay::Redraw() {
	if ( !m_is_redraw_needed ) {
		m_is_redraw_needed = true;
	}
}

bool Overlay::SceneBelongs( const scene::Scene* scene ) const {
	return scene->GetType() == scene::SCENE_TYPE_SIMPLE2D;
}

}
}
}
