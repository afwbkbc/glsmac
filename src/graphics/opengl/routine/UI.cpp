#include "UI.h"

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

UI::UI( OpenGL* opengl, shader_program::Simple2D* shader_program )
	: Routine( opengl )
	, m_shader_program( shader_program ) {
	//
}

void UI::Start() {

}

void UI::Stop() {

}

void UI::Iterate() {

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
		( *it )->Update();
	}

	glDisable( GL_DEPTH_TEST );

	//Log( "Redrawing UI" );
	for ( auto& scene : m_gl_scenes ) {
		switch ( scene->GetScene()->GetType() ) {
			case scene::SCENE_TYPE_UI: {
				scene->Draw( m_shader_program );
				break;
			}
			default: {
				THROW( "unknown scene type " + std::to_string( scene->GetScene()->GetType() ) );
			}
		}
		ASSERT( !glGetError(), "UI draw error" );
	}

	glEnable( GL_DEPTH_TEST );

}

opengl::Actor* UI::AddCustomActor( scene::actor::Actor* actor ) {
	THROW( "shouldn't be here" );
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

bool UI::SceneBelongs( const scene::Scene* scene ) const {
	return scene->GetType() == scene::SCENE_TYPE_UI;
}

}
}
}
