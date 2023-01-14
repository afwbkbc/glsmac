#include "MeshActor.h"

#include "scene/actor/MeshActor.h"
#include "shader_program/OrthographicOpenGLShaderProgram.h"
#include "shader_program/WorldOpenGLShaderProgram.h"

#include "types/Matrix44.h"
#include "engine/Engine.h"

using namespace scene;
using namespace mesh;

namespace renderer {
namespace opengl {

MeshActor::MeshActor( scene::actor::MeshActor *actor ) : Actor( actor ) {

	Log( "Creating OpenGL actor for " + actor->GetName() );

	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );
	
}

MeshActor::~MeshActor() {
	Log( "Destroying OpenGL actor" );

	glDeleteBuffers( 1, &m_ibo );
	glDeleteBuffers( 1, &m_vbo );

}

bool MeshActor::ReloadNeeded() {
	size_t mesh_updated_counter = ((scene::actor::MeshActor *)m_actor)->GetMesh()->UpdatedCount();
	if ( m_update_counter == mesh_updated_counter )
		return false;
	m_update_counter = mesh_updated_counter;
	return true;
}

void MeshActor::Load() {
	//Log( "Loading OpenGL actor" );

	auto *actor = (scene::actor::MeshActor *)m_actor;

	const auto *mesh = actor->GetMesh();

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, mesh->GetVertexDataSize(), (GLvoid *)mesh->GetVertexData(), GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexDataSize(), (GLvoid *)mesh->GetIndexData(), GL_STATIC_DRAW);

	m_ibo_size = mesh->GetIndexCount();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	const auto* texture = actor->GetTexture();
	if (texture) {
		g_engine->GetRenderer()->LoadTexture(texture);
	}
}

void MeshActor::Unload() {
	//Log( "Unloading OpenGL actor" );

	if ( m_actor ) {
		auto *actor = (scene::actor::MeshActor *)m_actor;

		/* it's better to keep everything loaded forever
		const auto* texture = actor->GetTexture();
		if (texture) {
			g_engine->GetRenderer()->UnloadTexture(texture);
		}*/
	}
}

void MeshActor::Draw( shader_program::OpenGLShaderProgram *shader_program ) {

	//Log( "Drawing" );

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );

	auto *actor = (scene::actor::MeshActor *)m_actor;
	const auto *texture = actor->GetTexture();
	
	shader_program->Enable();

	switch ( shader_program->GetType() ) {
		case ( shader_program::OpenGLShaderProgram::TYPE_ORTHO ): {
			auto *ortho_shader_program = (shader_program::OrthographicOpenGLShaderProgram *)shader_program;
			glUniform1f( ortho_shader_program->m_gl_uniforms.z_index, m_actor->GetPosition().z );
			/*types::Color tint_color = actor->GetTintColor();
			const GLfloat tint_color_data[4] = { tint_color.red, tint_color.green, tint_color.blue, tint_color.alpha };
			glUniform4fv( ortho_shader_program->m_gl_uniforms.tint, 1, tint_color_data );*/
			break;
		}
		case ( shader_program::OpenGLShaderProgram::TYPE_PERSP ): {
			/*auto *persp_shader_program = (shader_program::WorldOpenGLShaderProgram *)shader_program;

			types::Matrix44 matrix = m_actor->GetWorldMatrix();
			glUniformMatrix4fv( persp_shader_program->m_gl_uniforms.world, 1, GL_TRUE, (const GLfloat*)(&matrix));

			glUniform3f( persp_shader_program->m_gl_uniforms.light_color, 1.0, 1.0, 1.0 );
			glUniform1f( persp_shader_program->m_gl_uniforms.light_intensity, 1.0 );

		    //glUniform3f( persp_shader_program->m_gl_uniforms.campos, 0.0, 0.0, 0.0 );
*/
			break;

		}
	}

	/*
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(MyVertex), BUFFER_OFFSET(12));   //The starting point of normals, 12 bytes away
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(MyVertex), BUFFER_OFFSET(24));   //The starting point of texcoords, 24 bytes away
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
*/

	//glEnableClientState( GL_VERTEX_ARRAY );
	//glVertexPointer( 2, GL_FLOAT, 0, (void *) 0 );
	// enable shader program?

	//glBindTexture(GL_TEXTURE_2D, this->mMaterialTextureObjs[0]);
	//glActiveTexture(GL_TEXTURE0);

	if (texture) {
		g_engine->GetRenderer()->EnableTexture(texture);
	}

	/*
	math::Matrix44 matrix;

	matrix.Identity();
	matrix.ProjectionOrtho2D( 0.01f, 100.0f );

	glUniformMatrix4fv( m_shader_program->m_gl_uniforms.position, 1, GL_TRUE, (const GLfloat *) &matrix );*/

	//math::Vec2<> matrix( 0.0f, 0.0f );
	//glUniformMatrix2fv( m_shader_program->m_gl_uniforms.position, 1, GL_TRUE, (const GLfloat *) &matrix );

	//glUniformMatrix4fv(shader_program->mUWorld, 1, GL_TRUE, (const GLfloat*)(&this->mActorFinalMatrices[i]));

	glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );

	if (texture) {
		g_engine->GetRenderer()->DisableTexture();
	}

	shader_program->Disable();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

} /* namespace opengl */
} /* namespace renderer */
