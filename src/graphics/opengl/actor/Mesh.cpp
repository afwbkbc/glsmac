#include <iostream>

#include "Mesh.h"

#include "scene/actor/Mesh.h"
#include "scene/actor/Instanced.h"

#include "../shader_program/Orthographic.h"
#include "../shader_program/World.h"

#include "rr/GetData.h"

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
	const std::string last_texture_name = texture ? texture->m_name : "";
	
	if ( m_last_texture_name != last_texture_name ) {
		m_last_texture_name = last_texture_name;
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
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32UI, w, h, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL );
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

	//Log( "Drawing" );
	
	auto* mesh_actor = GetMeshActor();
	
	if ( shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO_DATA ) {
		if ( !mesh_actor->GetDataMesh() || !rr::HasRequests<rr::GetData>() ) {
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

	const auto* texture = mesh_actor->GetTexture();

	g_engine->GetGraphics()->EnableTexture(texture);
	
	switch ( shader_program->GetType() ) {
		case ( shader_program::ShaderProgram::TYPE_SIMPLE2D ) : {
			glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );
			break;
		}
		case ( shader_program::ShaderProgram::TYPE_ORTHO ):
		case ( shader_program::ShaderProgram::TYPE_ORTHO_DATA ): {
			auto* sp = (shader_program::Orthographic *)shader_program; // TODO: make base class for ortho and ortho_data
			
			auto flags = mesh_actor->GetRenderFlags();
			
			GLuint ibo_size = 0;
			
			switch ( shader_program->GetType() ) {
				case shader_program::ShaderProgram::TYPE_ORTHO: {
					// non-world uniforms apply only to render mesh
					glUniform1ui( sp->uniforms.flags, flags );
					auto* lights = m_actor->GetScene()->GetLights();
					if ( !( flags & actor::Mesh::RF_IGNORE_LIGHTING ) && !lights->empty() ) {
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
					if ( flags & actor::Mesh::RF_USE_TINT ) {
						glUniform4fv( sp->uniforms.tint_color, 1, (const GLfloat*)&mesh_actor->GetTintColor() );
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
			
			if ( flags & actor::Mesh::RF_IGNORE_DEPTH ) {
				glDisable( GL_DEPTH_TEST );
			}
			
			const bool ignore_camera = ( flags & scene::actor::Mesh::RF_IGNORE_CAMERA );
			
			if ( ignore_camera || m_actor->GetType() == scene::Actor::TYPE_MESH ) {
				types::Matrix44 matrix = ignore_camera
					? g_engine->GetUI()->GetWorldUIMatrix()
					: m_actor->GetWorldMatrix()
				;
				glUniformMatrix4fv( sp->uniforms.world, 1, GL_TRUE, (const GLfloat*)(&matrix));
				glDrawElements( GL_TRIANGLES, ibo_size, GL_UNSIGNED_INT, (void *)(0) );
			}
			else if ( m_actor->GetType() == scene::Actor::TYPE_INSTANCED_MESH ) {
				auto* instanced = (scene::actor::Instanced*) m_actor;
				auto& matrices = instanced->GetWorldMatrices();
				glUniformMatrix4fv( sp->uniforms.world, matrices.size(), GL_TRUE, (const GLfloat*)(matrices.data()));
				glDrawElementsInstanced( GL_TRIANGLES, ibo_size, GL_UNSIGNED_INT, (void *)(0), matrices.size() );
			}
			else {
				ASSERT( false, "unknown actor type " + std::to_string( m_actor->GetType() ) );
			}

			if ( flags & actor::Mesh::RF_IGNORE_DEPTH ) {
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
	
		auto requests = rr::GetRequests< rr::GetData >();
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

/**

[452x609] BEFORE: 0 0 0 0
[452x609] AFTER: 1479 1057280482 1060154032 0
[275x345] BEFORE: 0 0 0 0
[275x345] AFTER: 2194 1057087877 1060087377 0
[660x451] BEFORE: 0 0 0 0
[660x451] AFTER: 1965 0 0 0
[660x451] BEFORE: 0 0 0 0
[660x451] AFTER: 1965 0 0 0
[452x524] BEFORE: 0 0 0 0
[452x524] AFTER: 1799 0 0 0
[697x394] BEFORE: 0 0 0 0
[697x394] AFTER: 2125 0 1953 0
[396x337] BEFORE: 0 0 0 0
[396x337] AFTER: 2198 1005905590 1483 0
[409x634] BEFORE: 0 0 0 0
[409x634] AFTER: 1398 0 0 0
[802x617] BEFORE: 0 0 0 0
[802x617] AFTER: 1408 0 1404 0
[385x420] BEFORE: 0 0 0 0
[385x420] AFTER: 1957 0 1876 0
[385x420] BEFORE: 0 0 0 0
[385x420] AFTER: 1957 0 1955 0
[292x531] BEFORE: 0 0 0 0
[292x531] AFTER: 1635 1049057988 1052711678 0
[292x531] BEFORE: 0 0 0 0
[292x531] AFTER: 1635 0 0 0
[689x456] BEFORE: 0 0 0 0
[689x456] AFTER: 1965 0 0 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 981047768 1390 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1005435529 1171 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1028588231 1388 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1024004038 1244 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1028588231 1394 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 912 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1321 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1394 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1327 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1390 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1024004038 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1056265500 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1238 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1319 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1053585738 1323 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1388 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1236 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1024004038 1321 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1315 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 0 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1050302677 1157 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1317 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1016755134 1394 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1331 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1248 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1024004038 1388 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1240 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1056446297 1329 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1252 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1240 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1404 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1153 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1325 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1050302677 1388 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1327 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1052704223 1323 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 849 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1050302677 1392 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1024004038 1171 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1228 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1157 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1325 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1161 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1028588231 1236 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1329 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1321 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1394 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1250 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1056446297 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1327 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1027835463 920 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1001 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1024936973 1390 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1028590855 1327 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1252 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1327 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1388 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1317 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1388 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1052738340 1323 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1050302677 1244 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1016755134 1228 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 999768306 1392 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1032005045 1230 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1331 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1055648727 1317 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1398 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1068 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1049306904 1392 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1088 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 999768306 1404 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 1028588231 1321 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1307 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1394 0
[628x606] BEFORE: 0 0 0 0
[628x606] AFTER: 1404 0 1388 0

*/

/**

[768x509] BEFORE: 0 0 0 0
[768x509] AFTER: 1803 0 0 0
[994x524] BEFORE: 0 0 0 0
[994x524] AFTER: 1728 0 0 0
[994x524] BEFORE: 0 0 0 0
[994x524] AFTER: 1728 0 0 0
[669x419] BEFORE: 0 0 0 0
[669x419] AFTER: 2040 0 0 0
[1132x469] BEFORE: 0 0 0 0
[1132x469] AFTER: 1892 0 0 0
[1132x469] BEFORE: 0 0 0 0
[1132x469] AFTER: 1892 0 0 0
[1132x469] BEFORE: 0 0 0 0
[1132x469] AFTER: 1892 0 0 0
[1132x469] BEFORE: 0 0 0 0
[1132x469] AFTER: 1892 0 0 0
[1132x469] BEFORE: 0 0 0 0
[1132x469] AFTER: 1892 0 0 0
[1132x469] BEFORE: 0 0 0 0
[1132x469] AFTER: 1892 0 0 0

*/

	struct {
		uint32_t r = 0;
		uint32_t g = 0;
		uint32_t b = 0;
		uint32_t a = 0;
	} test = {};
	
	std::cout << "[" << x << "x" << y << "] " << "BEFORE: " << test.r << " " << test.g << " " << test.b << " " << test.a << std::endl;

	glReadPixels( x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &test );
	
	std::cout << "[" << x << "x" << y << "] " << "AFTER: " << test.r << " " << test.g << " " << test.b << " " << test.a << std::endl;

	ASSERT( !glGetError(), "Error reading data pixel" );
	
	return data;
}

} /* namespace opengl */
} /* namespace graphics */
