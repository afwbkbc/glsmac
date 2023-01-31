#include "Mesh.h"

#include "scene/actor/Mesh.h"
#include "../shader_program/Orthographic.h"
#include "../shader_program/World.h"

#include "types/Matrix44.h"
#include "engine/Engine.h"

using namespace scene;
using namespace mesh;

namespace graphics {
namespace opengl {

Mesh::Mesh( scene::actor::Mesh *actor ) : Actor( actor ) {

	//Log( "Creating OpenGL actor for " + actor->GetName() );

	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );
	
}

Mesh::~Mesh() {
	//Log( "Destroying OpenGL actor" );

	glDeleteBuffers( 1, &m_ibo );
	glDeleteBuffers( 1, &m_vbo );

}

bool Mesh::ReloadNeeded() {
	size_t mesh_updated_counter = ((scene::actor::Mesh *)m_actor)->GetMesh()->UpdatedCount();
	if ( m_update_counter == mesh_updated_counter )
		return false;
	m_update_counter = mesh_updated_counter;
	return true;
}

void Mesh::Load() {
	auto *actor = (scene::actor::Mesh *)m_actor;

	const auto *mesh = actor->GetMesh();
#ifdef DEBUG
	if ( !mesh ) {
		throw runtime_error( "actor mesh not set" );
	}
#endif

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, mesh->GetVertexDataSize(), (GLvoid *)ptr( mesh->GetVertexData(), 0, mesh->GetVertexDataSize() ), GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexDataSize(), (GLvoid *)ptr( mesh->GetIndexData(), 0, mesh->GetIndexDataSize() ), GL_STATIC_DRAW);

	m_ibo_size = mesh->GetIndexCount();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	const auto* texture = actor->GetTexture();
	if (texture) {
		g_engine->GetGraphics()->LoadTexture(texture);
	}
}

void Mesh::Unload() {
	// Log( "Unloading OpenGL actor" );

	if ( m_actor ) {
/*		auto *actor = (scene::actor::Mesh *)m_actor;

		// it's better to keep everything loaded forever
		const auto* texture = actor->GetTexture();
		if (texture) {
			g_engine->GetGraphics()->UnloadTexture(texture);
		}*/
	}
}

void Mesh::Draw( shader_program::ShaderProgram *shader_program, Camera *camera ) {

	//Log( "Drawing" );

	
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );

	shader_program->Enable();

	auto *actor = (scene::actor::Mesh *)m_actor;
	const auto *texture = actor->GetTexture();
	
	//shader_program->Enable();

	switch ( shader_program->GetType() ) {
		case ( shader_program::ShaderProgram::TYPE_ORTHO ): {
			auto *sp = (shader_program::Orthographic *)shader_program;

			
			types::Matrix44 matrix = m_actor->GetWorldMatrix();
			glUniformMatrix4fv( sp->m_gl_uniforms.world, 1, GL_TRUE, (const GLfloat*)(&matrix));

			auto* light = actor->GetScene()->GetLight();
			if ( light ) {
				glUniform3fv( sp->m_gl_uniforms.light_pos, 1, (const GLfloat*)&light->GetPosition() );
				glUniform4fv( sp->m_gl_uniforms.light_color, 1, (const GLfloat*)&light->GetColor() );
			}

			//glUniform1f( sp->m_gl_uniforms.z_index, m_actor->GetPosition().z );
			/*types::Color tint_color = actor->GetTintColor();
			const GLfloat tint_color_data[4] = { tint_color.red, tint_color.green, tint_color.blue, tint_color.alpha };
			glUniform4fv( sp->m_gl_uniforms.tint, 1, tint_color_data );*/
			break;
		}
		case ( shader_program::ShaderProgram::TYPE_PERSP ): {
			/*auto *persp_shader_program = (shader_program::World *)shader_program;

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

	g_engine->GetGraphics()->EnableTexture(texture);

	/*
	math::Matrix44 matrix;

	matrix.Identity();
	matrix.ProjectionOrtho2D( 0.01f, 100.0f );

	glUniformMatrix4fv( m_shader_program->m_gl_uniforms.position, 1, GL_TRUE, (const GLfloat *) &matrix );*/

	//math::Vec2<> matrix( 0.0f, 0.0f );
	//glUniformMatrix2fv( m_shader_program->m_gl_uniforms.position, 1, GL_TRUE, (const GLfloat *) &matrix );

	//glUniformMatrix4fv(shader_program->mUWorld, 1, GL_TRUE, (const GLfloat*)(&this->mActorFinalMatrices[i]));

	glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );

	g_engine->GetGraphics()->DisableTexture();

	shader_program->Disable();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
}

} /* namespace opengl */
} /* namespace graphics */
