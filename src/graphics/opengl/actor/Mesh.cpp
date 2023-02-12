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
	
	if ( m_data.is_allocated ) {
		glDeleteFramebuffers( 1, &m_data.fbo );
		glDeleteTextures( 1, &m_data.picking_texture );
		glDeleteTextures( 1, &m_data.depth_texture );
		glDeleteBuffers( 1, &m_data.vbo );
		glDeleteBuffers( 1, &m_data.ibo );
	}
}

bool Mesh::ReloadNeeded() {
	auto* actor = (scene::actor::Mesh *) m_actor;
	size_t mesh_updated_counter = actor->GetMesh()->UpdatedCount();
	auto *data_mesh = actor->GetDataMesh();
	if ( data_mesh ) {
		mesh_updated_counter = std::max( mesh_updated_counter, data_mesh->UpdatedCount() );
	}
	if ( m_update_counter == mesh_updated_counter ) {
		return false;
	}
	m_update_counter = mesh_updated_counter;
	return true;
}

void Mesh::Load() {
	auto *actor = (scene::actor::Mesh *)m_actor;

	const auto *mesh = actor->GetMesh();
	ASSERT( mesh, "actor mesh not set" );

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

void Mesh::PrepareDataMesh() {
	auto* actor = (scene::actor::Mesh *) m_actor;
	const auto *data_mesh = actor->GetDataMesh();
	if ( data_mesh && !m_data.is_up_to_date ) {
		if ( !m_data.is_allocated ) {
			Log( "Initializing data mesh" );
			glGenFramebuffers( 1, &m_data.fbo );
			glGenTextures( 1, &m_data.picking_texture );
			glGenTextures( 1, &m_data.depth_texture );
			glGenBuffers( 1, &m_data.vbo );
			glGenBuffers( 1, &m_data.ibo );

			glBindBuffer( GL_ARRAY_BUFFER, m_data.vbo );
			glBufferData( GL_ARRAY_BUFFER, data_mesh->GetVertexDataSize(), (GLvoid *)ptr( data_mesh->GetVertexData(), 0, data_mesh->GetVertexDataSize() ), GL_STATIC_DRAW );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_data.ibo );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, data_mesh->GetIndexDataSize(), (GLvoid *)ptr( data_mesh->GetIndexData(), 0, data_mesh->GetIndexDataSize() ), GL_STATIC_DRAW);

			m_data.ibo_size = data_mesh->GetIndexCount();

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			m_data.is_allocated = true;
		}
		
		size_t w = g_engine->GetGraphics()->GetViewportWidth();
		size_t h = g_engine->GetGraphics()->GetViewportHeight();
		
		Log( "(re)loading data mesh (viewport size: " + std::to_string( w ) + "x" + std::to_string( h ) + ")" );
		
		glBindFramebuffer( GL_FRAMEBUFFER, m_data.fbo );
		
		glBindTexture( GL_TEXTURE_2D, m_data.picking_texture );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32UI, w, h, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_data.picking_texture, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		
		glBindTexture( GL_TEXTURE_2D, m_data.depth_texture );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_data.depth_texture, 0 );
		glBindTexture( GL_TEXTURE_2D, 0 );
		
		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		ASSERT( status == GL_FRAMEBUFFER_COMPLETE, "FB error, status: " + std::to_string( status ) );
		
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );	

		m_data.is_up_to_date = true;
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
	
	auto* actor = (scene::actor::Mesh*) m_actor;
	
	if ( shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA ) {
		if ( !actor->GetDataMesh() || m_data.last_processed_data_request_id == actor->GetLastDataRequestId() ) {
			return; // nothing to do
		}
		PrepareDataMesh();
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_data.fbo );
		glDrawBuffer( GL_COLOR_ATTACHMENT0 );
		glBindBuffer( GL_ARRAY_BUFFER, m_data.vbo );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_data.ibo );
		
		// reset framebuffer to clean state
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	}
	else {
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	}
	
	shader_program->Enable();

	const auto* texture = actor->GetTexture();

	g_engine->GetGraphics()->EnableTexture(texture);
	
	switch ( shader_program->GetType() ) {
		case ( shader_program::ShaderProgram::TYPE_SIMPLE2D ) : {
			glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );
			break;
		}
		case ( shader_program::ShaderProgram::TYPE_ORTHO ):
		case ( shader_program::ShaderProgram::TYPE_ORTHO_DATA ): {
			auto* sp = (shader_program::Orthographic *)shader_program; // TODO: make base class for ortho and ortho_data
			
			// uniforms apply only to render mesh
			if ( shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO ) {
				
				auto* light = actor->GetScene()->GetLight();
				if ( light ) {
					glUniform3fv( sp->uniforms.light_pos, 1, (const GLfloat*)&light->GetPosition() );
					glUniform4fv( sp->uniforms.light_color, 1, (const GLfloat*)&light->GetColor() );
				}
				auto flags = actor->GetRenderFlags();
				glUniform1ui( sp->uniforms.flags, flags );
				if ( flags & actor::Mesh::RF_USE_TINT ) {
					glUniform4fv( sp->uniforms.tint_color, 1, (const GLfloat*)&actor->GetTintColor() );
				}
			}

			if ( actor->GetType() == scene::Actor::TYPE_INSTANCED_MESH ) {
				auto* instanced_actor = (scene::actor::InstancedMesh*) m_actor;
				auto& matrices = instanced_actor->GetWorldMatrices();
				glUniformMatrix4fv( sp->uniforms.world, matrices.size(), GL_TRUE, (const GLfloat*)(matrices.data()));
				glDrawElementsInstanced( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0), matrices.size() );
			}
			else {
				types::Matrix44 matrix = m_actor->GetWorldMatrix();
				glUniformMatrix4fv( sp->uniforms.world, 1, GL_TRUE, (const GLfloat*)(&matrix));
				glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );
			}

			break;
		}
		case ( shader_program::ShaderProgram::TYPE_PERSP ): {

			// TODO
			ASSERT( false, "perspective projection not implemented yet" );
			
			break;

		}
		default: {
			ASSERT( false, "shader program type " + std::to_string( shader_program->GetType() ) + " not implemented" );
		}
	}
	
	g_engine->GetGraphics()->DisableTexture();
	
	if ( shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA ) {
		
		glDrawBuffer( GL_NONE );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		
		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_data.fbo );
		glReadBuffer( GL_COLOR_ATTACHMENT0 );
	
		auto* requests = actor->GetDataRequests();
		for ( auto& r : *requests ) {
			if ( !r.second.is_processed ) {
				// Log( "Processing data request " + to_string( r.first ) );
				r.second.result = GetDataAt( r.second.screen_x, r.second.screen_inverse_y );
				r.second.is_processed = true;
			}
		}
		
		glReadBuffer( GL_NONE );
		glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
	
		// processed all pending requests
		m_data.last_processed_data_request_id = actor->GetLastDataRequestId();
		
	}
	
	shader_program->Disable();
	
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
}

void Mesh::OnResize() {
	if ( m_data.is_allocated ) {
		m_data.is_up_to_date = false; // need to recreate fbo for new window size
	}
}

mesh::Data::data_t Mesh::GetDataAt( const size_t x, const size_t y ) {
	ASSERT( m_data.is_allocated, "mesh data not allocated" );
	
	mesh::Data::data_t data = 0;
	
	glReadPixels( x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &data );
	
	ASSERT( !glGetError(), "Error reading data pixel" );
	
	return data;
}

} /* namespace opengl */
} /* namespace graphics */
