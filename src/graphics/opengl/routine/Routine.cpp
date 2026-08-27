#include <algorithm>

#include "Routine.h"

#include "scene/Scene.h"
#include "graphics/Graphics.h"

namespace graphics {
namespace opengl {
namespace routine {

Routine::Routine( OpenGL* opengl )
	: m_opengl( opengl ) {
	//
}

bool Routine::AddScene( scene::Scene* scene ) {
	if ( SceneBelongs( scene ) ) {
		ASSERT( std::find( m_scenes.begin(), m_scenes.end(), scene ) == m_scenes.end(), "duplicate scene add" );

		m_scenes.push_back( scene );

		NEWV( gl_scene, opengl::Scene, m_opengl, scene, this );

		m_gl_scenes.push_back( gl_scene );

		//Log( "Scene [" + scene->GetName() + "] added" );

		return true;
	}
	return false;
}

bool Routine::RemoveScene( scene::Scene* scene ) {
	if ( SceneBelongs( scene ) ) {
		auto it = std::find( m_scenes.begin(), m_scenes.end(), scene );
		if ( it < m_scenes.end() ) {
			const auto scene_index = it - m_scenes.begin();
			auto gl_scene_it = m_gl_scenes.begin() + scene_index;

			OnSceneRemove( *gl_scene_it );

			DELETE( *gl_scene_it );
			m_gl_scenes.erase( gl_scene_it );
			m_scenes.erase( it );

			//Log( "Scene [" + scene->GetName() + "] removed" );

			return true;
		}
	}
	return false;
}

Routine::~Routine() {
	for ( auto& s : m_gl_scenes ) {
		DELETE( s );
	}
}

void Routine::OnWindowResize() {
	for ( auto& s : m_gl_scenes ) {
		s->OnWindowResize();
	}
}

}
}
}
