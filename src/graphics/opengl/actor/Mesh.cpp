#include "Mesh.h"

#include "scene/actor/Mesh.h"
#include "scene/actor/Instanced.h"

#include "../shader_program/Orthographic.h"
#include "../shader_program/World.h"

#include "rr/GetData.h"
#include "rr/Capture.h"

#include "types/Matrix44.h"
#include "engine/Engine.h"

using namespace scene;
using namespace mesh;

namespace graphics {
namespace opengl {

Mesh::Mesh( scene::actor::Actor *actor ) : Actor( actor ) {

	//Log( "Creating OpenGL actor for " + actor->GetName() );

	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );
	
}

Mesh::~Mesh() {
	//Log( "Destroying OpenGL actor" );

	glDeleteBuffers( 1, &m_ibo );
	glDeleteBuffers( 1, &m_vbo );
	
	if ( m_data.is_allocated ) {
		glBindFramebuffer( GL_FRAMEBUFFER, m_data.fbo );
		glDeleteTextures( 1, &m_data.picking_texture );
		glDeleteTextures( 1, &m_data.depth_texture );
		glDeleteBuffers( 1, &m_data.vbo );
		glDeleteBuffers( 1, &m_data.ibo );
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glDeleteFramebuffers( 1, &m_data.fbo );
	}
}

bool Mesh::MeshReloadNeeded() {
	auto* actor = GetMeshActor();
	size_t mesh_updated_counter = actor->GetMesh()->UpdatedCount();
	auto *data_mesh = actor->GetDataMesh();
	if ( data_mesh ) {
		mesh_updated_counter = mesh_updated_counter + data_mesh->UpdatedCount();
	}
	
	if ( m_mesh_update_counter != mesh_updated_counter ) {
		m_mesh_update_counter = mesh_updated_counter;
		return true;
	}
	return false;
}

bool Mesh::TextureReloadNeeded() {
	auto* texture = GetMeshActor()->GetTexture();
	if ( m_last_texture != texture ) {
		m_last_texture = texture;
		return true;
	}
	return false;
}

void Mesh::LoadMesh() {
	
	//Log( "Loading mesh" );
	
	const auto *mesh = GetMeshActor()->GetMesh();
	ASSERT( mesh, "actor mesh not set" );

	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, mesh->GetVertexDataSize(), (GLvoid *)ptr( mesh->GetVertexData(), 0, mesh->GetVertexDataSize() ), GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexDataSize(), (GLvoid *)ptr( mesh->GetIndexData(), 0, mesh->GetIndexDataSize() ), GL_STATIC_DRAW );

	m_ibo_size = mesh->GetIndexCount();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
}

void Mesh::LoadTexture() {
	const auto* texture = GetMeshActor()->GetTexture();
	
	if ( texture ) {
		g_engine->GetGraphics()->LoadTexture( texture );
	}
}

scene::actor::Mesh* Mesh::GetMeshActor() const {
	switch ( m_actor->GetType() ) {
		case scene::Actor::TYPE_MESH: {
			return (scene::actor::Mesh*)m_actor;
			break;
		}
		case scene::Actor::TYPE_INSTANCED_MESH: {
			return ((scene::actor::Instanced*)m_actor)->GetMeshActor();
			break;
		}
		default: {
			ASSERT( false, "unknown actor type " + std::to_string( m_actor->GetType() ) );
			return nullptr;
		}
	}
}

void Mesh::PrepareDataMesh() {
	const auto *data_mesh = GetMeshActor()->GetDataMesh();
	if ( data_mesh && !m_data.is_up_to_date ) {
		if ( !m_data.is_allocated ) {
			
			Log( "Initializing data mesh" );
			
			glGenFramebuffers( 1, &m_data.fbo );
			glBindFramebuffer( GL_FRAMEBUFFER, m_data.fbo );
			
			glGenBuffers( 1, &m_data.vbo );
			glBindBuffer( GL_ARRAY_BUFFER, m_data.vbo );
			glBufferData( GL_ARRAY_BUFFER, data_mesh->GetVertexDataSize(), (GLvoid *)ptr( data_mesh->GetVertexData(), 0, data_mesh->GetVertexDataSize() ), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &m_data.ibo );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_data.ibo );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, data_mesh->GetIndexDataSize(), (GLvoid *)ptr( data_mesh->GetIndexData(), 0, data_mesh->GetIndexDataSize() ), GL_STATIC_DRAW);
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			m_data.ibo_size = data_mesh->GetIndexCount();
		}
		else {
			glBindFramebuffer( GL_FRAMEBUFFER, m_data.fbo );
		}
		
		size_t w = g_engine->GetGraphics()->GetViewportWidth();
		size_t h = g_engine->GetGraphics()->GetViewportHeight();
		
		Log( "(re)loading data mesh (viewport size: " + std::to_string( w ) + "x" + std::to_string( h ) + ")" );
		
		if ( !m_data.is_allocated ) {
			glGenTextures( 1, &m_data.picking_texture );
		}
		glBindTexture( GL_TEXTURE_2D, m_data.picking_texture );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_R32UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_data.picking_texture, 0 );
		
		if ( !m_data.is_allocated ) {
			glGenTextures( 1, &m_data.depth_texture );
		}
		glBindTexture( GL_TEXTURE_2D, m_data.depth_texture );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
		glBindTexture( GL_TEXTURE_2D, 0 );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_data.depth_texture, 0 );
		
#ifdef DEBUG
		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		ASSERT( status == GL_FRAMEBUFFER_COMPLETE, "FB error, status: " + std::to_string( status ) );
#endif
		
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );	

		m_data.is_up_to_date = true;
		if ( !m_data.is_allocated ) {
			m_data.is_allocated = true;
		}
	}

}

void Mesh::Draw( shader_program::ShaderProgram *shader_program, Camera *camera ) {

l_draw_begin:
	
	//Log( "Drawing" );
	
	auto* mesh_actor = GetMeshActor();
	
	rr::Capture* capture_request = nullptr;
	FBO* fbo = nullptr;
	
	if ( shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA ) {
		if ( !mesh_actor->GetDataMesh() || !mesh_actor->RR_HasRequests<rr::GetData>() ) {
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
		
		if ( mesh_actor->RR_HasRequests< rr::Capture >() ) {
			Log( "Creating FBO for capture" );
			
			// process only one per Draw() because cameras may differ
			// in vast majority of cases only one call will be here anyway
			capture_request = mesh_actor->RR_GetRequests< rr::Capture >().front();
			ASSERT( capture_request->camera, "capture request without camera" );
			
			NEW( fbo, FBO, capture_request->texture_width, capture_request->texture_height );
			fbo->WriteBegin();
			
		}
		
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	}
	
	shader_program->Enable();

	const auto* texture = mesh_actor->GetTexture();
	auto flags = mesh_actor->GetRenderFlags();
	
	g_engine->GetGraphics()->EnableTexture(texture);
	
	switch ( shader_program->GetType() ) {
		case ( shader_program::ShaderProgram::TYPE_SIMPLE2D ) : {
			auto* sp = (shader_program::Simple2D *)shader_program;
			glUniform1ui( sp->uniforms.flags, flags );
			if ( flags & actor::Actor::RF_USE_COORDINATE_LIMITS ) {
				const auto& limits = mesh_actor->GetCoordinateLimits();
				glUniform3fv( sp->uniforms.coordinate_limits.min, 1, (const GLfloat*)&limits.first );
				glUniform3fv( sp->uniforms.coordinate_limits.max, 1, (const GLfloat*)&limits.second );
			}
			glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );
			break;
		}
		case ( shader_program::ShaderProgram::TYPE_ORTHO ):
		case ( shader_program::ShaderProgram::TYPE_ORTHO_DATA ): {
			auto* sp = (shader_program::Orthographic *)shader_program; // TODO: make base class for ortho and ortho_data
			
			GLuint ibo_size = 0;
			
			switch ( shader_program->GetType() ) {
				case shader_program::ShaderProgram::TYPE_ORTHO: {
					// non-world uniforms apply only to render mesh
					glUniform1ui( sp->uniforms.flags, flags );
					auto* lights = m_actor->GetScene()->GetLights();
					if ( !( flags & actor::Actor::RF_IGNORE_LIGHTING ) && !lights->empty() ) {
						Vec3 light_pos[ lights->size() ];
						Color light_color[ lights->size() ];
						size_t i = 0;
						for ( auto& light : *lights ) {
							light_pos[ i ] = light->GetPosition();
							light_color[ i ] = light->GetColor();
							i++;
						}
						glUniform3fv( sp->uniforms.light_pos, lights->size(), (const GLfloat*)light_pos );
						glUniform4fv( sp->uniforms.light_color, lights->size(), (const GLfloat*)light_color );
					}
					if ( flags & actor::Actor::RF_USE_TINT ) {
						glUniform4fv( sp->uniforms.tint_color, 1, (const GLfloat*)&mesh_actor->GetTintColor() );
					}
					if ( flags & actor::Actor::RF_USE_COORDINATE_LIMITS ) {
						const auto& limits = mesh_actor->GetCoordinateLimits();
						glUniform3fv( sp->uniforms.coordinate_limits.min, 1, (const GLfloat*)&limits.first );
						glUniform3fv( sp->uniforms.coordinate_limits.max, 1, (const GLfloat*)&limits.second );
					}
					ibo_size = m_ibo_size;
					break;
				}
				case shader_program::ShaderProgram::TYPE_ORTHO_DATA: {
					ibo_size = m_data.ibo_size;
					break;
				}
				default: {
					ASSERT( false, "unknown shader program type " + std::to_string( shader_program->GetType() ) );
				}
			}
			
			if ( flags & actor::Actor::RF_IGNORE_DEPTH ) {
				glDisable( GL_DEPTH_TEST );
			}
			
			const bool ignore_camera =
				( flags & scene::actor::Actor::RF_IGNORE_CAMERA )// ||
				//fbo
			;
			
			// TODO: instanced capture_request ?
			if ( ignore_camera || m_actor->GetType() == scene::Actor::TYPE_MESH ) {
				types::Matrix44 matrix;
				ASSERT( !capture_request, "non-instanced captures not implemented" );
				if ( ignore_camera ) {
					matrix = g_engine->GetUI()->GetWorldUIMatrix();
				}
				else {
					matrix = m_actor->GetWorldMatrix();
				}
				glUniformMatrix4fv( sp->uniforms.world, 1, GL_TRUE, (const GLfloat*)(&matrix));
				glDrawElements( GL_TRIANGLES, ibo_size, GL_UNSIGNED_INT, (void *)(0) );
			}
			else if ( m_actor->GetType() == scene::Actor::TYPE_INSTANCED_MESH ) {
				auto* instanced = (scene::actor::Instanced*) m_actor;
				scene::actor::Instanced::world_matrices_t matrices;
				if ( capture_request ) {
					matrices = instanced->GenerateWorldMatrices( capture_request->camera );
				}
				else {
					matrices = instanced->GetWorldMatrices();
				}
				glUniformMatrix4fv( sp->uniforms.world, matrices.size(), GL_TRUE, (const GLfloat*)(matrices.data()));
				glDrawElementsInstanced( GL_TRIANGLES, ibo_size, GL_UNSIGNED_INT, (void *)(0), matrices.size() );
			}
			else {
				ASSERT( false, "unknown actor type " + std::to_string( m_actor->GetType() ) );
			}

			if ( flags & actor::Actor::RF_IGNORE_DEPTH ) {
				glEnable( GL_DEPTH_TEST );
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
	
	shader_program->Disable();
	
	if ( shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA ) {
		
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		
		glDrawBuffer( GL_NONE );
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		
		glBindFramebuffer( GL_READ_FRAMEBUFFER, m_data.fbo );
		glReadBuffer( GL_COLOR_ATTACHMENT0 );
	
		auto requests = mesh_actor->RR_GetRequests< rr::GetData >();
		for ( auto& r : requests ) {
			r->data = GetDataAt( r->screen_x, r->screen_inverse_y );
			r->SetProcessed();
		}
		
		glReadBuffer( GL_NONE );
		glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );

	}
	else {
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		
		if ( mesh_actor->RR_HasRequests< rr::Capture >() ) {

			fbo->WriteEnd();
		
			capture_request->texture = fbo->CaptureToTexture();
			capture_request->SetProcessed();
			
			Log( "Destroying FBO for capture" );
			DELETE( fbo );
			
			// this draw was captured into texture, draw for real now (or process next capture if there are more)
			goto l_draw_begin;
		}
	}
}

void Mesh::OnResize() {
	if ( m_data.is_allocated ) {
		m_data.is_up_to_date = false; // need to recreate fbo for new window size
	}
}

mesh::Data::data_t Mesh::GetDataAt( const size_t x, const size_t y ) {
	ASSERT( m_data.is_allocated, "mesh data not allocated" );
	
	mesh::Data::data_t data = 0;
	glReadPixels( x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &data );
	ASSERT( !glGetError(), "Error reading data pixel" );
	
	return data;
}

} /* namespace opengl */
} /* namespace graphics */
