#include "Cache.h"

#include <algorithm>

#include "engine/Engine.h"
#include "graphics/opengl/OpenGL.h"
#include "graphics/opengl/FBO.h"
#include "graphics/opengl/shader_program/Simple2D.h"
#include "scene/actor/Cache.h"
#include "types/mesh/Rectangle.h"
#include "types/texture/Texture.h"

namespace graphics {
namespace opengl {

Cache::Cache( OpenGL* opengl, scene::actor::Actor* actor )
	: Actor( AT_CACHE, opengl, actor ) {
	ASSERT( actor->GetType() == scene::actor::Actor::TYPE_CACHE, "invalid actor type" );
	m_mesh = new types::mesh::Rectangle();
	glGenBuffers( 1, &m_vbo );
	glGenBuffers( 1, &m_ibo );
}

Cache::~Cache() {
	if ( !m_cache_children.empty() ) {
		Log( "WARNING: cache children not empty" );
		for ( const auto& c : m_cache_children ) {
			Log( c->GetName() );
		}
	}
	glDeleteBuffers( 1, &m_ibo );
	glDeleteBuffers( 1, &m_vbo );
	if ( m_texture ) {
		delete m_texture;
	}
	delete m_mesh;
}

void Cache::AddCacheChild( Actor* cache_child ) {
	ASSERT( m_cache_children.find( cache_child ) == m_cache_children.end(), "duplicate cache child" );
	m_cache_children.insert( cache_child );
	AddCacheChildToZIndexSet( cache_child, cache_child->GetZIndex() );
	UpdateCache();
}

void Cache::RemoveCacheChild( Actor* cache_child ) {
	ASSERT( m_cache_children.find( cache_child ) != m_cache_children.end(), "cache child not found" );
	RemoveCacheChildFromZIndexSet( cache_child, cache_child->GetZIndex() );
	m_cache_children.erase( cache_child );
	UpdateCache();
}

void Cache::SetCacheChildZIndex( Actor* cache_child, const float zindex ) {
	RemoveCacheChildFromZIndexSet( cache_child, cache_child->GetZIndex() );
	AddCacheChildToZIndexSet( cache_child, zindex );
	UpdateCache();
}

void Cache::UpdateCache() {
	if ( !m_is_update_needed ) {
		m_is_update_needed = true;
	}
}

void Cache::OnWindowResize() {
	UpdateCache();
}

void Cache::UpdateCacheImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera, const bool force ) {

	auto* actor = (scene::actor::Cache*)m_actor;
	if ( !m_is_update_needed ) {
		const auto update_counter = actor->GetUpdateCounter();
		if ( force || update_counter != m_update_counter ) {
			m_update_counter = update_counter;
			m_is_update_needed = true;
		}
	}
	if ( m_is_update_needed ) {

		m_is_update_needed = false;

		types::Vec2< size_t > top_left, bottom_right;
		actor->GetEffectiveArea( top_left, bottom_right );

		const bool need_update = !m_texture || force || top_left != m_top_left || bottom_right != m_bottom_right;

		for ( const auto& it : m_cache_children_by_zindex ) {
			for ( const auto& child : it.second ) {
				if ( child->m_type == AT_CACHE ) {
					( (Cache*)child )->UpdateCacheImpl( shader_program, camera, need_update );
				}
			}
		}

		if ( need_update ) {

			//Log( "Resizing/realigning cache" );

			m_top_left = top_left;
			m_bottom_right = bottom_right;
			if ( !m_texture ) {
				m_texture = new types::texture::Texture();
			}

			auto tl = m_opengl->GetGLCoords( m_top_left );
			auto br = m_opengl->GetGLCoords( m_bottom_right );

			// TODO: investigate and fix properly
			const auto tmp = br.y;
			br.y = tl.y;
			tl.y = tmp;

			m_mesh->SetCoords( tl, br, m_z_index );
			m_opengl->WithBindBuffers(
				m_vbo, m_ibo, [ this ]() {
					glBufferData( GL_ARRAY_BUFFER, m_mesh->GetVertexDataSize(), (GLvoid*)ptr( m_mesh->GetVertexData(), 0, m_mesh->GetVertexDataSize() ), GL_STATIC_DRAW );
					glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_mesh->GetIndexDataSize(), (GLvoid*)ptr( m_mesh->GetIndexData(), 0, m_mesh->GetIndexDataSize() ), GL_STATIC_DRAW );
				}
			);
			m_ibo_size = m_mesh->GetIndexCount();
		}

		//Log( "Updating cache texture" );

		// TODO: investigate and fix properly
		auto tl = m_top_left;
		auto br = m_bottom_right;
		const auto tmp = br.y;
		const auto vh = m_opengl->GetViewportHeight();
		br.y = vh > tl.y
			? vh - tl.y
			: 0;
		tl.y = vh > tmp
			? vh - tmp
			: 0;

		m_opengl->CaptureToTexture(
			m_texture, tl, br, [ this, &shader_program, &camera ]() {
				for ( const auto& it : m_cache_children_by_zindex ) {
					for ( const auto& child : it.second ) {
						if ( child->GetActor()->IsVisible() ) {
							child->DrawImpl( shader_program, camera );
						}
					}
				}
			}
		);

		if ( !m_texture->IsEmpty() ) {
/*			const types::Color c = types::Color::FromRGB( rand() % 256, rand() % 256, rand() % 256 );
			for ( auto y = 0 ; y < m_texture->m_height ; y++ ) {
				m_texture->SetPixel( 0, y, c );
				m_texture->SetPixel( m_texture->m_width - 1, y, c );
			}
			for ( auto x = 0 ; x < m_texture->m_width ; x++ ) {
				m_texture->SetPixel( x, 0, c );
				m_texture->SetPixel( x, m_texture->m_height - 1, c );
			}*/
			m_opengl->LoadTexture( m_texture, false );
		}

	}
}

void Cache::DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {
	ASSERT( shader_program->GetType() == shader_program::ShaderProgram::TYPE_SIMPLE2D, "invalid shader program type" );

	if ( m_texture && !m_texture->IsEmpty() ) {
		m_opengl->WithBindBuffers(
			m_vbo, m_ibo, [ this, &shader_program ]() {

				m_opengl->WithShaderProgram(
					shader_program, [ this, &shader_program ]() {

						m_opengl->WithTexture(
							m_texture, [ this, &shader_program ]() {

								auto* sp = (shader_program::Simple2D*)shader_program;
								glUniform1ui( sp->uniforms.flags, scene::actor::Actor::RF_NONE );
								glDrawElements( GL_TRIANGLES, m_ibo_size, GL_UNSIGNED_INT, (void*)( 0 ) );

							}
						);
					}
				);
			}
		);
	}
}

void Cache::AddCacheChildToZIndexSet( Actor* gl_actor, const float zindex ) {
	auto it = m_cache_children_by_zindex.find( zindex );
	if ( it == m_cache_children_by_zindex.end() ) {
		m_cache_children_by_zindex[ zindex ] = {};
		it = m_cache_children_by_zindex.find( zindex );
	}
	ASSERT( std::find( it->second.begin(), it->second.end(), gl_actor ) == it->second.end(), "actor already found in zindex set" );
	//Log( "Adding actor " + gl_actor->GetName() + " to zindex set " + std::to_string( zindex ) );
	it->second.push_back( gl_actor );
}

void Cache::RemoveCacheChildFromZIndexSet( Actor* gl_actor, const float zindex ) {
	//Log( "Removing actor " + gl_actor->GetName() + " from zindex set " + std::to_string( zindex ) );
	auto it = m_cache_children_by_zindex.find( zindex );
	ASSERT( it != m_cache_children_by_zindex.end(), "zindex set not found" );
	auto actor_it = find( it->second.begin(), it->second.end(), gl_actor );
	ASSERT( actor_it != it->second.end(), "actor not found in zindex set" );
	it->second.erase( actor_it );
}

}
}
