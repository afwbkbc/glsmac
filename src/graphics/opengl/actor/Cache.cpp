#include "Cache.h"

#include <algorithm>

#include "engine/Engine.h"
#include "graphics/opengl/OpenGL.h"
#include "graphics/opengl/FBO.h"
#include "graphics/opengl/shader_program/Orthographic.h"
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
	m_is_update_needed = true;
}

void Cache::RemoveCacheChild( Actor* cache_child ) {
	ASSERT( m_cache_children.find( cache_child ) != m_cache_children.end(), "cache child not found" );
	RemoveCacheChildFromZIndexSet( cache_child, cache_child->GetZIndex() );
	m_cache_children.erase( cache_child );
	m_is_update_needed = true;
}

void Cache::SetCacheChildZIndex( Actor* cache_child, const float zindex ) {
	RemoveCacheChildFromZIndexSet( cache_child, cache_child->GetZIndex() );
	AddCacheChildToZIndexSet( cache_child, zindex );
	m_is_update_needed = true;
}

void Cache::OnWindowResize() {
	m_is_update_needed = true;
}

const bool Cache::UpdateCacheImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera, const bool force ) {

	auto* actor = (scene::actor::Cache*)m_actor;

	m_is_update_needed |= force || !m_texture;
	if ( !m_is_update_needed ) {
		const auto update_counter = actor->GetUpdateCounter();
		if ( update_counter != m_update_counter ) {
			m_update_counter = update_counter;
			m_is_update_needed = true;
		}
	}

	types::Vec2< size_t > top_left, bottom_right;
	actor->GetEffectiveArea( top_left, bottom_right );

	if ( !m_is_update_needed ) {
		const auto oldw = m_bottom_right.x - m_top_left.x;
		const auto oldh = m_bottom_right.y - m_top_left.y;
		const auto neww = bottom_right.x - top_left.x;
		const auto newh = bottom_right.y - top_left.y;
		m_is_update_needed = oldw != neww || oldh != newh;
	}

	if ( m_is_update_needed || // on window resize tl and br may change while m_top_left and m_bottom_right stay the same
		m_top_left != top_left ||
		m_bottom_right != bottom_right
		) {
		m_top_left = top_left;
		m_bottom_right = bottom_right;
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
	}

	if ( m_is_update_needed ) {

		m_is_update_needed = false;

		for ( const auto& it : m_cache_children_by_zindex ) {
			for ( const auto& child : it.second ) {
				if ( child->m_type == AT_CACHE ) {
					( (Cache*)child )->UpdateCacheImpl( shader_program, camera, false );
				}
			}
		}

		if ( !m_texture ) {
			m_texture = new types::texture::Texture();
		}

		// TODO: investigate and fix properly
		auto tl2 = m_top_left;
		auto br2 = m_bottom_right;
		const auto tmp2 = br2.y;
		const auto vh2 = m_opengl->GetViewportHeight();
		br2.y = vh2 > tl2.y
			? vh2 - tl2.y
			: 0;
		tl2.y = vh2 > tmp2
			? vh2 - tmp2
			: 0;

		m_opengl->CaptureToTexture(
			m_texture, tl2, br2, [ this, &shader_program, &camera ]() {
				auto* const scene = this->GetActor()->GetScene();
				for ( const auto& it : m_cache_children_by_zindex ) {
					for ( const auto& child : it.second ) {
						auto* const actor = child->GetActor();
						if ( actor->GetScene() == scene ) {
							if ( actor->IsVisible() ) {
								child->DrawImpl( shader_program, camera );
							}
						}
					}
				}
			}
		);
		if ( !m_texture->IsEmpty() ) {
			m_opengl->LoadTexture( m_texture, false );
		}

		return true;
	}

	return false;
}

void Cache::DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {
	ASSERT( shader_program->GetType() == shader_program::ShaderProgram::TYPE_ORTHO, "invalid shader program type" );

	if ( m_texture && !m_texture->IsEmpty() ) {
		m_opengl->WithBindBuffers(
			m_vbo, m_ibo, [ this, &shader_program ]() {

				m_opengl->WithShaderProgram(
					shader_program, [ this, &shader_program ]() {

						m_opengl->WithTexture(
							m_texture, [ this, &shader_program ]() {

								auto* sp = (shader_program::Orthographic*)shader_program;
								glUniform1ui(
									sp->uniforms.flags,
									scene::actor::Actor::RF_IGNORE_CAMERA |
										scene::actor::Actor::RF_IGNORE_LIGHTING
								);
								m_ibo_size = m_mesh->GetIndexCount();
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
