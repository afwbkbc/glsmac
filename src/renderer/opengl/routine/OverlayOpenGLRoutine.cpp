#include "OverlayOpenGLRoutine.h"

namespace renderer {
namespace opengl {
namespace routine {

void OverlayOpenGLRoutine::Start() {

}

void OverlayOpenGLRoutine::Stop() {

}

void OverlayOpenGLRoutine::Iterate() {

	//glEnable( GL_BLEND );
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it )
		(*it)->Update();

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
		(*it)->Draw( m_shader_program );
	}

	//glDisable( GL_BLEND );

}

bool OverlayOpenGLRoutine::SceneBelongs( const scene::Scene *scene ) const {
	return scene->GetType() == scene::SCENE_TYPE_ORTHO;
}

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */
