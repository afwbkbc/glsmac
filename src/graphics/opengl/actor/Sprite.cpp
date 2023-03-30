#include "Sprite.h"

#include "scene/actor/Sprite.h"
#include "scene/actor/Instanced.h"

#include "../shader_program/Orthographic.h"
#include "../shader_program/World.h"

#include "types/Matrix44.h"

#include "engine/Engine.h"

#include "types/mesh/Render.h"

using namespace scene;
using namespace mesh;

namespace graphics {
namespace opengl {

Sprite::Sprite( scene::actor::Actor *actor ) : Actor( actor ) {

	//Log( "Creating OpenGL actor for " + actor->GetName() );

	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );
	
}

Sprite::~Sprite() {
	//Log( "Destroying OpenGL actor" );

	glDeleteBuffers( 1, &m_ibo );
	glDeleteBuffers( 1, &m_vbo );
	
}

bool Sprite::MeshReloadNeeded() {
	auto* actor = GetSpriteActor();
	return
		m_last_dimensions != actor->GetDimensions() ||
		m_last_tex_coords != actor->GetTexCoords()
	;
}

bool Sprite::TextureReloadNeeded() {
	auto* texture = GetSpriteActor()->GetTexture();
	
	if ( m_last_texture != texture ) {
		m_last_texture = texture;
		return true;
	}
	return false;
}

void Sprite::LoadMesh() {
	auto *actor = GetSpriteActor();

	m_last_dimensions = actor->GetDimensions();
	m_last_tex_coords = actor->GetTexCoords();

	auto* mesh = actor->GenerateMesh();
	
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, mesh->GetVertexDataSize(), (GLvoid *)ptr( mesh->GetVertexData(), 0, mesh->GetVertexDataSize() ), GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexDataSize(), (GLvoid *)ptr( mesh->GetIndexData(), 0, mesh->GetIndexDataSize() ), GL_STATIC_DRAW );

	m_ibo_size = mesh->GetIndexCount();

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	DELETE( mesh );
}

void Sprite::LoadTexture() {
	auto* texture = GetSpriteActor()->GetTexture();
	
	if ( texture ) {
		g_engine->GetGraphics()->LoadTexture( texture );
	}
}

scene::actor::Sprite* Sprite::GetSpriteActor() const {
	switch ( m_actor->GetType() ) {
		case scene::Actor::TYPE_SPRITE: {
			return (scene::actor::Sprite*)m_actor;
			break;
		}
		case scene::Actor::TYPE_INSTANCED_SPRITE: {
			return ((scene::actor::Instanced*)m_actor)->GetSpriteActor();
			break;
		}
		default: {
			ASSERT( false, "unknown actor type " + std::to_string( m_actor->GetType() ) );
			return nullptr;
		}
	}
}

void Sprite::Draw( shader_program::ShaderProgram *shader_program, Camera *camera ) {

	auto* sprite_actor = GetSpriteActor();
	
	//Log( "Drawing" );
	
	glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	
	shader_program->Enable();

	const auto* texture = sprite_actor->GetTexture();

	g_engine->GetGraphics()->EnableTexture(texture);
	
	switch ( shader_program->GetType() ) {
		case ( shader_program::ShaderProgram::TYPE_ORTHO_DATA ): {
			// no data for sprites
			break;
		}
		case ( shader_program::ShaderProgram::TYPE_ORTHO ): {
			auto* sp = (shader_program::Orthographic *)shader_program;
			
			auto flags = sprite_actor->GetRenderFlags();
			glUniform1ui( sp->uniforms.flags, flags );
			if ( flags & actor::Actor::RF_USE_2D_POSITION ) {
				glUniform2fv( sp->uniforms.position, 1, (const GLfloat*)&sprite_actor->GetPosition() );
			}
			
			auto* lights = m_actor->GetScene()->GetLights();
			if ( !lights->empty() ) {
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
			
			glDisable( GL_DEPTH_TEST );
			
			glUniformMatrix4fv( sp->uniforms.world, 1, GL_TRUE, (const GLfloat*)&camera->GetMatrix() );
			
			if ( m_actor->GetType() == scene::Actor::TYPE_SPRITE ) {
				types::Matrix44 matrix = m_actor->GetWorldMatrix();
				glUniformMatrix4fv( sp->uniforms.instances, 1, GL_TRUE, (const GLfloat*)(&matrix));
				glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0) );
			}
			else if ( m_actor->GetType() == scene::Actor::TYPE_INSTANCED_SPRITE ) {
				auto* instanced = (scene::actor::Instanced*) m_actor;
				auto& matrices = instanced->GetInstanceMatrices();
				const auto sz = matrices.size();
				GLsizei i = 0;
				GLsizei c;
				for ( auto i = 0 ; i < sz ; i += OpenGL::MAX_INSTANCES ) {
					c = std::min< size_t >( OpenGL::MAX_INSTANCES, sz - i );
					glUniformMatrix4fv( sp->uniforms.instances, c, GL_TRUE, (const GLfloat*) ( matrices.data() + i ) );
					glDrawElementsInstanced( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void *)(0), c );
				}
			}
			else {
				ASSERT( false, "unknown actor type " + std::to_string( m_actor->GetType() ) );
			}

			glEnable( GL_DEPTH_TEST );
			
			break;
		}
		
/*		case ( shader_program::ShaderProgram::TYPE_PERSP ): {

			// TODO
			ASSERT( false, "perspective projection not implemented yet" );
			
			break;

		}*/
		default: {
			ASSERT( false, "shader program type " + std::to_string( shader_program->GetType() ) + " not implemented" );
		}
	}
	
	g_engine->GetGraphics()->DisableTexture();
	
	shader_program->Disable();
	
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

} /* namespace opengl */
} /* namespace graphics */
