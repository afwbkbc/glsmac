#include "Mesh.h"

#include "scene/actor/Mesh.h"
#include "scene/actor/InstancedMesh.h"
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

	auto* actor = (scene::actor::Mesh*) m_actor;
	const auto* texture = actor->GetTexture();

	g_engine->GetGraphics()->EnableTexture(texture);
	
	switch ( shader_program->GetType() ) {
		case ( shader_program::ShaderProgram::TYPE_SIMPLE2D ) : {
			glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );
			break;
		}
		case ( shader_program::ShaderProgram::TYPE_ORTHO ): {
			auto* sp = (shader_program::Orthographic *)shader_program;
			
			if ( actor->GetType() == scene::Actor::TYPE_INSTANCED_MESH ) {
				// TODO: fix water transparency artifacts
				auto* instanced_actor = (scene::actor::InstancedMesh*) m_actor;
				auto& matrices = instanced_actor->GetWorldMatrices();
				glUniformMatrix4fv( sp->m_gl_uniforms.world, matrices.size(), GL_TRUE, (const GLfloat*)(matrices.data()));
				glDrawElementsInstanced( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0), matrices.size() );
			}
			else {
				types::Matrix44 matrix = m_actor->GetWorldMatrix();
				glUniformMatrix4fv( sp->m_gl_uniforms.world, 1, GL_TRUE, (const GLfloat*)(&matrix));
				glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );
			}

			auto* light = actor->GetScene()->GetLight();
			if ( light ) {
				glUniform3fv( sp->m_gl_uniforms.light_pos, 1, (const GLfloat*)&light->GetPosition() );
				glUniform4fv( sp->m_gl_uniforms.light_color, 1, (const GLfloat*)&light->GetColor() );
			}

			break;
		}
		case ( shader_program::ShaderProgram::TYPE_PERSP ): {

			// TODO
			ASSERT( false, "perspective projection not implemented yet" );
			
			break;

		}
		default: {
			ASSERT( false, "shader program type " + to_string( shader_program->GetType() ) + " not implemented" );
		}
	}

	g_engine->GetGraphics()->DisableTexture();

	shader_program->Disable();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
}

} /* namespace opengl */
} /* namespace graphics */
