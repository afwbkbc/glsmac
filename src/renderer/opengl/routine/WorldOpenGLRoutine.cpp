#include "WorldOpenGLRoutine.h"

namespace renderer {
namespace opengl {
namespace routine {

void WorldOpenGLRoutine::Start() {

}

void WorldOpenGLRoutine::Stop() {

}

void WorldOpenGLRoutine::Iterate() {

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it )
		(*it)->Update();

	glEnable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
/*	glEnable(GL_LIGHT0);
	float LightPos[4]={-5.0f,5.0f,10.0f,0.0f};
	float Ambient[4]={0.5f,0.5f,0.5f,1.0f};
	glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
	glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);

    glEnable( GL_FOG );
    glHint( GL_FOG_HINT, GL_NICEST );
    glFogf( GL_FOG_DENSITY, 5.0f );
    glFogf( GL_FOG_START, 0 );
    glFogf( GL_FOG_END, 4);
    float fogColor[] = {1.0f, 0, 0, 0.5f };
    glFogfv( GL_FOG_COLOR, fogColor );
    // Select how fog distance is calculated.
    glFogi( GL_FOG_COORD_SRC, GL_FOG_COORD );
    //glFogi( GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH );*/

    GLint OldCullFaceMode; // possibly hack
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    glCullFace(GL_FRONT);

	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
		(*it)->Draw( m_shader_program );
	}

    glCullFace(OldCullFaceMode);

	//glDisable( GL_FOG );

	glDisable(GL_LIGHTING);

	glDisable(GL_CULL_FACE);
}

bool WorldOpenGLRoutine::SceneBelongs( const scene::Scene *scene ) const {
	return scene->GetType() == scene::SCENE_TYPE_PERSP;
}

} /* namespace routine */
} /* namespace opengl */
} /* namespace renderer */
