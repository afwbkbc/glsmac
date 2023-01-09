#include "MeshActor.h"

#include "scene/actor/MeshActor.h"
#include "shader_program/OrthographicOpenGLShaderProgram.h"
#include "shader_program/WorldOpenGLShaderProgram.h"

#include "types/Matrix44.h"

namespace renderer {
namespace opengl {

MeshActor::MeshActor( scene::actor::MeshActor *actor ) : Actor( actor ) {

	Log( "Creating OpenGL actor" );

	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );
	/*
	for (int i=0;i<this->mModel.mMaterials.size();i++) {
		this->mMaterialTextureObjs.push_back(0);
		this->mRenderer->ActivateTexture(&this->mModel.mMaterials[i]->mTexture1Map.texture,&this->mMaterialTextureObjs[i]);
	}*/

	//glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	//glBindBuffer( GL_ARRAY_BUFFER, 0 );

	m_update_timestamp = actor->GetMesh()->UpdatedAt();
}

MeshActor::~MeshActor() {
	Log( "Destroying OpenGL actor" );

	glDeleteBuffers( 1, &m_ibo );
	glDeleteBuffers( 1, &m_vbo );

}

bool MeshActor::ReloadNeeded() {
	size_t mesh_updated_at = ((scene::actor::MeshActor *)m_actor)->GetMesh()->UpdatedAt();
	if ( m_update_timestamp == mesh_updated_at )
		return false;
	m_update_timestamp = mesh_updated_at;
	return true;
}

void MeshActor::Load() {
	//Log( "(re)loading OpenGL actor" );

	scene::actor::MeshActor *mesh_actor = (scene::actor::MeshActor *)m_actor;

	scene::mesh::Mesh *mesh = mesh_actor->GetMesh();

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, mesh->GetVertexData()->size() * sizeof(scene::mesh::Mesh::coord_t), (GLvoid *)mesh->GetVertexData()->data(), GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexData()->size() * sizeof(scene::mesh::Mesh::index_t), (GLvoid *)mesh->GetIndexData()->data(), GL_STATIC_DRAW);

	m_ibo_size = mesh->GetIndexData()->size();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void MeshActor::Unload() {
	//Log( "Destroying OpenGL actor" );

	/*for (int i=0;i<this->mModel.mMaterials.size();i++)
		this->mRenderer->DeactivateTexture(&this->mModel.mMaterials[i]->mTexture1Map.texture,&this->mMaterialTextureObjs[i]);*/

}

void MeshActor::Draw( shader_program::OpenGLShaderProgram *shader_program ) {

	//Log( "Drawing" );

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );

	shader_program->Enable();

	switch ( shader_program->GetType() ) {
		case ( shader_program::OpenGLShaderProgram::TYPE_ORTHO ): {
			auto *ortho_shader_program = (shader_program::OrthographicOpenGLShaderProgram *)shader_program;
			glUniform1f( ortho_shader_program->m_gl_uniforms.z_index, m_actor->GetPosition().z );
			types::Color tint_color = ((scene::actor::MeshActor *)m_actor)->GetTintColor();
			const GLfloat tint_color_data[4] = { tint_color.red, tint_color.green, tint_color.blue, tint_color.alpha };
			glUniform4fv( ortho_shader_program->m_gl_uniforms.tint, 1, tint_color_data );
			break;
		}
		case ( shader_program::OpenGLShaderProgram::TYPE_PERSP ): {
			auto *persp_shader_program = (shader_program::WorldOpenGLShaderProgram *)shader_program;

			types::Matrix44 matrix = m_actor->GetWorldMatrix();
			glUniformMatrix4fv( persp_shader_program->m_gl_uniforms.world, 1, GL_TRUE, (const GLfloat*)(&matrix));

			glUniform3f( persp_shader_program->m_gl_uniforms.light_color, 1.0, 1.0, 1.0 );
			glUniform1f( persp_shader_program->m_gl_uniforms.light_intensity, 1.0 );

		    //glUniform3f( persp_shader_program->m_gl_uniforms.campos, 0.0, 0.0, 0.0 );

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

/*
	math::Matrix44 matrix;

	matrix.Identity();
	matrix.ProjectionOrtho2D( 0.01f, 100.0f );

	glUniformMatrix4fv( m_shader_program->m_gl_uniforms.position, 1, GL_TRUE, (const GLfloat *) &matrix );*/

	//math::Vec2<> matrix( 0.0f, 0.0f );
	//glUniformMatrix2fv( m_shader_program->m_gl_uniforms.position, 1, GL_TRUE, (const GLfloat *) &matrix );

	//glUniformMatrix4fv(shader_program->mUWorld, 1, GL_TRUE, (const GLfloat*)(&this->mActorFinalMatrices[i]));

	glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );

	shader_program->Disable();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

} /* namespace opengl */
} /* namespace renderer */
