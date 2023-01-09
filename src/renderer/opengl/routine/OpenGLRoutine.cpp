#include <algorithm>

#include "OpenGLRoutine.h"

#include "renderer/Renderer.h"

namespace renderer {
namespace opengl {
namespace routine {

bool OpenGLRoutine::AddScene( scene::Scene *scene ) {
	if ( SceneBelongs( scene )) {

		if ( std::find( m_scenes.begin(), m_scenes.end(), scene ) < m_scenes.end() )
			throw RendererError( "duplicate scene add" );

		m_scenes.push_back( scene );

		opengl::Scene *gl_scene = new opengl::Scene( scene, this );

		m_gl_scenes.push_back( gl_scene );

		Log("Scene [" + scene->GetName() + "] added");

		return true;
	}
	return false;
}

bool OpenGLRoutine::RemoveScene( scene::Scene *scene ) {
	if ( SceneBelongs( scene )) {
		auto it = std::find( m_scenes.begin(), m_scenes.end(), scene );
		if ( it < m_scenes.end() ) {

			m_scenes.erase( it, it + 1 );
			auto gl_scene_index = m_gl_scenes.begin() + ( it - m_scenes.begin() );

			OnSceneRemove( *gl_scene_index );

			delete (*gl_scene_index);
			m_gl_scenes.erase( gl_scene_index, gl_scene_index + 1 );

			Log("Scene [" + scene->GetName() + "] removed");

			return true;
		}
	}
	return false;
}

OpenGLRoutine::~OpenGLRoutine() {
	for ( auto it = m_gl_scenes.begin() ; it != m_gl_scenes.end() ; ++it )
		delete (*it);
}

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */
