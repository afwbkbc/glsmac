#include "Mesh.h"

#include "scene/Scene.h"
#include "scene/Light.h"
#include "scene/Camera.h"
#include "scene/actor/Actor.h"
#include "scene/actor/Mesh.h"
#include "scene/actor/Instanced.h"
#include "graphics/Graphics.h"
#include "graphics/opengl/OpenGL.h"
#include "graphics/opengl/FBO.h"
#include "graphics/opengl/shader_program/Orthographic.h"
#include "graphics/opengl/shader_program/OrthographicData.h"
#include "graphics/opengl/shader_program/Simple2D.h"
#include "graphics/opengl/shader_program/World.h"
#include "rr/GetData.h"
#include "rr/Capture.h"
#include "types/Matrix44.h"
#include "types/texture/Texture.h"
#include "types/mesh/Mesh.h"
#include "types/mesh/Data.h"
#include "engine/Engine.h"
#include "ui_legacy/UI.h"

namespace graphics {
namespace opengl {

Mesh::Mesh( OpenGL* opengl, scene::actor::Actor* actor )
	: Actor( AT_MESH, opengl, actor ) {

	//Log( "Creating OpenGL actor for " + actor->GetName() );

	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );

}

Mesh::~Mesh() {
	//Log( "Destroying OpenGL actor" );

	glDeleteBuffers( 1, &m_ibo );
	glDeleteBuffers( 1, &m_vbo );

	if ( m_data.is_allocated ) {
		m_opengl->WithBindFramebuffer(
			GL_FRAMEBUFFER, m_data.fbo, [ this ]() {
				glDeleteTextures( 1, &m_data.picking_texture );
				glDeleteTextures( 1, &m_data.depth_texture );
				glDeleteBuffers( 1, &m_data.vbo );
				glDeleteBuffers( 1, &m_data.ibo );
			}
		);
		glDeleteFramebuffers( 1, &m_data.fbo );
	}
}

bool Mesh::MeshReloadNeeded() {
	auto* actor = GetMeshActor();
	auto* mesh = actor->GetMesh();
	ASSERT( mesh, "actor has no mesh" );
	size_t mesh_updated_counter = mesh->UpdatedCount();
	if ( m_mesh_update_counter != mesh_updated_counter ) {
		//Log( "Mesh reload needed ( " + std::to_string( m_mesh_update_counter ) + " != " + std::to_string( mesh_updated_counter ) + " )" );
		m_mesh_update_counter = mesh_updated_counter;
		return true;
	}
	return DataMeshReloadNeeded();
}

bool Mesh::DataMeshReloadNeeded() {
	auto* actor = GetMeshActor();
	auto* data_mesh = actor->GetDataMesh();
	if ( data_mesh ) {
		size_t mesh_updated_counter = data_mesh->UpdatedCount();
		if ( m_data_mesh_update_counter != mesh_updated_counter ) {
			//Log( "Data mesh reload needed ( " + std::to_string( m_data_mesh_update_counter ) + " != " + std::to_string( mesh_updated_counter ) + " )" );
			m_data_mesh_update_counter = mesh_updated_counter;
			m_data.is_up_to_date = false;
			return true;
		}
	}
	return false;
}

bool Mesh::TextureReloadNeeded() {
	auto* texture = GetMeshActor()->GetTexture();
	if ( m_last_texture != texture ) {
		//Log( "Texture reload needed ( texture was replaced )" );
		m_last_texture = texture;
		m_last_texture_update_counter = texture
			? texture->UpdatedCount()
			: 0;
		return true;
	}
	if ( texture ) {
		size_t last_texture_update_counter = texture->UpdatedCount();
		if ( m_last_texture_update_counter != last_texture_update_counter ) {
			//Log( "Texture reload needed ( " + std::to_string( m_last_texture_update_counter ) + " != " + std::to_string( last_texture_update_counter ) + " )" );
			m_last_texture_update_counter = last_texture_update_counter;
			return true;
		}
	}
	return false;
}

void Mesh::LoadMesh() {

	//Log( "Loading mesh" );

	const auto* mesh = GetMeshActor()->GetMesh();
	ASSERT( mesh, "actor mesh not set" );

	m_opengl->WithBindBuffers(
		m_vbo, m_ibo, [ &mesh ]() {
			glBufferData( GL_ARRAY_BUFFER, mesh->GetVertexDataSize(), (GLvoid*)ptr( mesh->GetVertexData(), 0, mesh->GetVertexDataSize() ), GL_STATIC_DRAW );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexDataSize(), (GLvoid*)ptr( mesh->GetIndexData(), 0, mesh->GetIndexDataSize() ), GL_STATIC_DRAW );
		}
	);
	m_ibo_size = mesh->GetIndexCount();

}

void Mesh::LoadTexture() {
	auto* texture = GetMeshActor()->GetTexture();

	if ( texture ) {
		g_engine->GetGraphics()->LoadTexture( texture );
	}
}

scene::actor::Mesh* Mesh::GetMeshActor() const {
	switch ( m_actor->GetType() ) {
		case scene::actor::Actor::TYPE_MESH: {
			return (scene::actor::Mesh*)m_actor;
			break;
		}
		case scene::actor::Actor::TYPE_INSTANCED_MESH: {
			return ( (scene::actor::Instanced*)m_actor )->GetMeshActor();
			break;
		}
		default: {
			THROW( "unknown actor type " + std::to_string( m_actor->GetType() ) );
			return nullptr;
		}
	}
}

void Mesh::PrepareDataMesh() {
	const auto* data_mesh = GetMeshActor()->GetDataMesh();
	if ( data_mesh && !m_data.is_up_to_date ) {
		if ( !m_data.is_allocated ) {

			Log( "Initializing data mesh" );

			glGenFramebuffers( 1, &m_data.fbo );

			m_opengl->WithBindFramebuffer(
				GL_FRAMEBUFFER, m_data.fbo, [ this ]() {

					glGenBuffers( 1, &m_data.vbo );
					glGenBuffers( 1, &m_data.ibo );

				}
			);

			m_data.ibo_size = data_mesh->GetIndexCount();
		}

		m_opengl->WithBindFramebuffer(
			GL_FRAMEBUFFER, m_data.fbo, [ this, &data_mesh ]() {

				m_opengl->WithBindBuffers(
					m_data.vbo, m_data.ibo, [ &data_mesh ]() {
						glBufferData( GL_ARRAY_BUFFER, data_mesh->GetVertexDataSize(), (GLvoid*)ptr( data_mesh->GetVertexData(), 0, data_mesh->GetVertexDataSize() ), GL_STATIC_DRAW );
						glBufferData( GL_ELEMENT_ARRAY_BUFFER, data_mesh->GetIndexDataSize(), (GLvoid*)ptr( data_mesh->GetIndexData(), 0, data_mesh->GetIndexDataSize() ), GL_STATIC_DRAW );
					}
				);

				size_t w = g_engine->GetGraphics()->GetViewportWidth();
				size_t h = g_engine->GetGraphics()->GetViewportHeight();

				Log( "(re)loading data mesh (viewport size: " + std::to_string( w ) + "x" + std::to_string( h ) + ")" );

				if ( !m_data.is_allocated ) {
					glGenTextures( 1, &m_data.picking_texture );
				}

				m_opengl->WithBindTexture(
					m_data.picking_texture, [ &w, &h ]() {
						glTexImage2D( GL_TEXTURE_2D, 0, GL_R32UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
						glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
					}
				);
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_data.picking_texture, 0 );

				if ( !m_data.is_allocated ) {
					glGenTextures( 1, &m_data.depth_texture );
				}
				m_opengl->WithBindTexture(
					m_data.depth_texture, [ &w, &h ]() {
						glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
					}
				);
				glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_data.depth_texture, 0 );

#ifdef DEBUG
				GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
				ASSERT( status == GL_FRAMEBUFFER_COMPLETE, "FB error, status: " + std::to_string( status ) );
#endif

			}
		);

		m_data.is_up_to_date = true;
		if ( !m_data.is_allocated ) {
			m_data.is_allocated = true;
		}
	}

}

void Mesh::OnWindowResize() {
	if ( m_data.is_allocated ) {
		m_data.is_up_to_date = false; // need to recreate fbo for new window size
	}
}

void Mesh::DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {

	l_draw_begin:

	//Log( "Drawing" );

	auto* mesh_actor = GetMeshActor();

	rr::Capture* capture_request = nullptr;
	FBO* fbo = nullptr;

	GLuint vbo, ibo;
	if ( shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA ) {
		if ( !mesh_actor->GetDataMesh() || !mesh_actor->RR_HasRequests< rr::GetData >() ) {
			return; // nothing to do
		}
		PrepareDataMesh();
		m_opengl->WithBindFramebufferBegin( GL_DRAW_FRAMEBUFFER, m_data.fbo );
		glDrawBuffer( GL_COLOR_ATTACHMENT0 );

		// reset framebuffer to clean state
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		vbo = m_data.vbo;
		ibo = m_data.ibo;
	}
	else {

		if ( mesh_actor->RR_HasRequests< rr::Capture >() ) {
			Log( "Creating FBO for capture" );

			// process only one per Draw() because cameras may differ
			// in vast majority of cases only one call will be here anyway
			capture_request = mesh_actor->RR_GetRequests< rr::Capture >().front();
			ASSERT( capture_request->camera, "capture request without camera" );

			NEW( fbo, FBO, m_opengl, capture_request->texture_width, capture_request->texture_height );
			fbo->WriteBegin();

		}

		vbo = m_vbo;
		ibo = m_ibo;
	}

	m_opengl->WithBindBuffers(
		vbo, ibo, [ this, &shader_program, &mesh_actor, &capture_request, &camera ]() {

			m_opengl->WithShaderProgram(
				shader_program, [ this, &shader_program, &mesh_actor, &capture_request, &camera ]() {

					const auto* texture = mesh_actor->GetTexture();
					auto flags = mesh_actor->GetRenderFlags();

					g_engine->GetGraphics()->WithTexture(
						texture, [ this, &shader_program, &flags, &mesh_actor, &capture_request, &camera ]() {

							const auto sptype = shader_program->GetType();
							switch ( sptype ) {
								case ( shader_program::ShaderProgram::TYPE_SIMPLE2D ) : {
									auto* sp = (shader_program::Simple2D*)shader_program;
									glUniform1ui( sp->uniforms.flags, flags );
									if ( flags & scene::actor::Actor::RF_USE_TINT ) {
										glUniform4fv( sp->uniforms.tint_color, 1, (const GLfloat*)&mesh_actor->GetTintColor().value );
									}
									if ( flags & scene::actor::Actor::RF_USE_AREA_LIMITS ) {
										const auto& limits = mesh_actor->GetAreaLimits();
										glUniform3fv( sp->uniforms.area_limits.min, 1, (const GLfloat*)&limits.first );
										glUniform3fv( sp->uniforms.area_limits.max, 1, (const GLfloat*)&limits.second );
									}
									if ( flags & scene::actor::Actor::RF_USE_2D_POSITION ) {
										glUniform2fv( sp->uniforms.position, 1, (const GLfloat*)&mesh_actor->GetPosition() );
									}
									glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void*)( 0 ) );
									break;
								}
								case ( shader_program::ShaderProgram::TYPE_ORTHO ):
								case ( shader_program::ShaderProgram::TYPE_ORTHO_DATA ): {
									auto* sp = sptype == shader_program::ShaderProgram::TYPE_ORTHO
										? (shader_program::Orthographic*)shader_program
										: nullptr;
									auto* sp_data = sptype == shader_program::ShaderProgram::TYPE_ORTHO_DATA
										? (shader_program::OrthographicData*)shader_program
										: nullptr;

									GLuint ibo_size;

									switch ( shader_program->GetType() ) {
										case shader_program::ShaderProgram::TYPE_ORTHO: {
											// non-world uniforms apply only to render mesh
											glUniform1ui( sp->uniforms.flags, flags );
											auto* lights = m_actor->GetScene()->GetLights();
											if ( !( flags & scene::actor::Actor::RF_IGNORE_LIGHTING ) && !lights->empty() ) {
												types::Vec3 light_pos[lights->size()];
												types::Color::color_t light_color[lights->size()];
												size_t i = 0;
												for ( auto& light : *lights ) {
													light_pos[ i ] = light->GetPosition();
													light_color[ i ] = light->GetColor();
													i++;
												}
												glUniform3fv( sp->uniforms.light_pos, lights->size(), (const GLfloat*)light_pos );
												glUniform4fv( sp->uniforms.light_color, lights->size(), (const GLfloat*)light_color );
											}
											if ( flags & scene::actor::Actor::RF_USE_TINT ) {
												glUniform4fv( sp->uniforms.tint_color, 1, (const GLfloat*)&mesh_actor->GetTintColor().value );
											}
											if ( flags & scene::actor::Actor::RF_USE_AREA_LIMITS ) {
												const auto& limits = mesh_actor->GetAreaLimits();
												glUniform3fv( sp->uniforms.area_limits.min, 1, (const GLfloat*)&limits.first );
												glUniform3fv( sp->uniforms.area_limits.max, 1, (const GLfloat*)&limits.second );
											}
											if ( flags & scene::actor::Actor::RF_USE_2D_POSITION ) {
												glUniform2fv( sp->uniforms.position, 1, (const GLfloat*)&mesh_actor->GetPosition() );
											}
											ibo_size = m_ibo_size;
											break;
										}
										case shader_program::ShaderProgram::TYPE_ORTHO_DATA: {
											ibo_size = m_data.ibo_size;
											break;
										}
										default: {
											THROW( "unknown shader program type " + std::to_string( shader_program->GetType() ) );
										}
									}

									if ( flags & scene::actor::Actor::RF_IGNORE_DEPTH ) {
										glDisable( GL_DEPTH_TEST );
									}

									const bool ignore_camera =
										( flags & scene::actor::Actor::RF_IGNORE_CAMERA )// ||
									//fbo
									;

									// TODO: instanced capture_request ?
									if ( !ignore_camera ) {
										glUniformMatrix4fv(
											shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA
												? sp_data->uniforms.world
												: sp->uniforms.world, 1, GL_TRUE, (const GLfloat*)(
												capture_request
													? &( capture_request->camera->GetMatrix() )
													: &( camera->GetMatrix() )
											)
										);
									}
									if ( ignore_camera || m_actor->GetType() == scene::actor::Actor::TYPE_MESH ) {
										types::Matrix44 matrix;
										ASSERT( !capture_request, "non-instanced captures not implemented" );
										if ( ignore_camera ) {
											matrix = g_engine->GetUI()->GetWorldUIMatrix();
										}
										else {
											matrix = m_actor->GetWorldMatrix();
										}
										glUniformMatrix4fv(
											shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA
												? sp_data->uniforms.instances
												: sp->uniforms.instances, 1, GL_TRUE, (const GLfloat*)( &matrix )
										);
										glDrawElements( GL_TRIANGLES, ibo_size, GL_UNSIGNED_INT, (void*)( 0 ) );
									}
									else if ( m_actor->GetType() == scene::actor::Actor::TYPE_INSTANCED_MESH ) {
										auto* instanced = (scene::actor::Instanced*)m_actor;
										scene::actor::Instanced::matrices_t matrices;
										if ( capture_request ) {
											instanced->GenerateInstanceMatrices( &matrices, capture_request->camera );
										}
										else {
											matrices = instanced->GetInstanceMatrices();
										}
										const auto sz = matrices.size();
										GLsizei i = 0;
										GLsizei c;
										for ( auto i = 0 ; i < sz ; i += OpenGL::MAX_INSTANCES ) {
											c = std::min< size_t >( OpenGL::MAX_INSTANCES, sz - i );
											glUniformMatrix4fv(
												shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA
													? sp_data->uniforms.instances
													: sp->uniforms.instances, c, GL_TRUE, (const GLfloat*)( matrices.data() + i )
											);
											glDrawElementsInstanced( GL_TRIANGLES, ibo_size, GL_UNSIGNED_INT, (void*)( 0 ), c );
										}
									}
									else {
										THROW( "unknown actor type " + std::to_string( m_actor->GetType() ) );
									}

									if ( flags & scene::actor::Actor::RF_IGNORE_DEPTH ) {
										glEnable( GL_DEPTH_TEST );
									}

									break;
								}
								case ( shader_program::ShaderProgram::TYPE_PERSP ): {

									// TODO
									THROW( "perspective projection not implemented yet" );

									break;

								}
								default: {
									THROW( "shader program type " + std::to_string( shader_program->GetType() ) + " not implemented" );
								}
							}

						}
					);
				}
			);
		}
	);

	if ( shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA ) {

		glDrawBuffer( GL_NONE );
		m_opengl->WithBindFramebufferEnd( GL_DRAW_FRAMEBUFFER );

		m_opengl->WithBindFramebuffer(
			GL_READ_FRAMEBUFFER, m_data.fbo, [ this, &mesh_actor ]() {

				glReadBuffer( GL_COLOR_ATTACHMENT0 );

				auto requests = mesh_actor->RR_GetRequests< rr::GetData >();
				for ( auto& r : requests ) {
					r->data = GetDataAt( r->screen_x, r->screen_inverse_y );
					r->SetProcessed();
				}

				glReadBuffer( GL_NONE );
			}
		);

	}
	else {

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

types::mesh::data_t Mesh::GetDataAt( const size_t x, const size_t y ) {
	ASSERT( m_data.is_allocated, "mesh data not allocated" );

	types::mesh::data_t data = 0;
	glReadPixels( x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &data );
	ASSERT( !glGetError(), "Error reading data pixel" );

	return data;
}

}
}
