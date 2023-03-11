#include <algorithm>

#include "Routine.h"

#include "graphics/Graphics.h"

namespace graphics {
namespace opengl {
namespace routine {

Routine::Routine( OpenGL* opengl )
	: m_opengl( opengl )
{
	//
}

bool Routine::AddScene( scene::Scene *scene ) {
	if ( SceneBelongs( scene )) {
		ASSERT( std::find( m_scenes.begin(), m_scenes.end(), scene ) == m_scenes.end(), "duplicate scene add" );

		m_scenes.push_back( scene );

		NEWV( gl_scene, opengl::Scene, scene, this );

		m_gl_scenes.push_back( gl_scene );

		Log("Scene [" + scene->GetName() + "] added");

		return true;
	}
	return false;
}

bool Routine::RemoveScene( scene::Scene *scene ) {
	if ( SceneBelongs( scene )) {
		auto it = std::find( m_scenes.begin(), m_scenes.end(), scene );
		if ( it < m_scenes.end() ) {

			m_scenes.erase( it, it + 1 );
			auto gl_scene_index = m_gl_scenes.begin() + ( it - m_scenes.begin() );

			OnSceneRemove( *gl_scene_index );

			DELETE( *gl_scene_index );
			m_gl_scenes.erase( gl_scene_index, gl_scene_index + 1 );

			Log("Scene [" + scene->GetName() + "] removed");

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

} /* namespace routine */
} /* namespace opengl */
} /* namespace graphics */
