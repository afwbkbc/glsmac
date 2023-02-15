#include "World.h"

namespace graphics {
namespace opengl {
namespace routine {

World::World(
	OpenGL* opengl,
	const scene::scene_type_t scene_type,
	shader_program::Orthographic* shader_program,
	shader_program::OrthographicData* data_shader_program
)
	: Routine( opengl )
	, m_scene_type( scene_type )
	, m_shader_program( shader_program )
	, m_data_shader_program( data_shader_program )
{
	//
}

void World::Start() {

}

void World::Stop() {

}

void World::Iterate() {
	
	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it )
		(*it)->Update();
	
	glClear( GL_DEPTH_BUFFER_BIT );
	
	glEnable(GL_CULL_FACE);
/*
    glEnable( GL_FOG );
    glHint( GL_FOG_HINT, GL_NICEST );
    glFogf( GL_FOG_DENSITY, 5.0f );
    glFogf( GL_FOG_START, 0 );
    glFogf( GL_FOG_END, 4);
    float fogColor[] = {1.0f, 0, 0, 0.5f };
    glFogfv( GL_FOG_COLOR, fogColor );
    // Select how fog distance is calculated.
    glFogi( GL_FOG_COORD_SRC, GL_FOG_COORD );
    //glFogi( GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH );
*/
	
    glCullFace(GL_FRONT);
	
	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
		(*it)->Draw( m_shader_program );
		ASSERT( !glGetError(), "World draw error" );
	}

/*
	glDisable( GL_FOG );
*/
	glDisable(GL_CULL_FACE);
	
	for ( auto it = m_gl_scenes.begin() ; it < m_gl_scenes.end() ; ++it ) {
		(*it)->Draw( m_data_shader_program );
		ASSERT( !glGetError(), "World data draw error" );
	}

}

bool World::SceneBelongs( const scene::Scene *scene ) const {
	//return scene->GetType() == scene::SCENE_TYPE_PERSP; // TODO: support both
	return scene->GetType() == m_scene_type;
}

} /* namespace routine */
} /* namespace opengl */
} /* namespace graphics */
