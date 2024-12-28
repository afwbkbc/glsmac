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

	m_fbo = new FBO( m_opengl, 0, 0 );
	m_mesh = new types::mesh::Rectangle();
}

Cache::~Cache() {
	if ( !m_cache_children.empty() ) {
		Log( "WARNING: cache children not empty" );
		for ( const auto& c : m_cache_children ) {
			Log( c->GetName() );
		}
	}
	if ( m_texture ) {
		delete m_texture;
	}
	delete m_mesh;
	delete m_fbo;
}

void Cache::AddCacheChild( Actor* cache_child ) {
	ASSERT( m_cache_children.find( cache_child ) == m_cache_children.end(), "duplicate cache child" );
	m_cache_children.insert( cache_child );
	AddCacheChildToZIndexSet( cache_child, cache_child->GetZIndex() );
}

void Cache::RemoveCacheChild( Actor* cache_child ) {
	ASSERT( m_cache_children.find( cache_child ) != m_cache_children.end(), "cache child not found" );
	RemoveCacheChildFromZIndexSet( cache_child, cache_child->GetZIndex() );
	m_cache_children.erase( cache_child );
}

void Cache::SetCacheChildZIndex( Actor* cache_child, const float zindex ) {
	RemoveCacheChildFromZIndexSet( cache_child, cache_child->GetZIndex() );
	AddCacheChildToZIndexSet( cache_child, zindex );
}

void Cache::UpdateCache() {
	if ( !m_is_update_needed ) {
		m_is_update_needed = true;
	}
}

void Cache::OnWindowResize() {
	UpdateCache();
}

void Cache::SetEffectiveArea( const types::Vec2< types::mesh::coord_t >& top_left, const types::Vec2< types::mesh::coord_t >& bottom_right, const types::mesh::coord_t z ) {
	m_mesh->SetCoords( top_left, bottom_right, z );
}

void Cache::UpdateCacheImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {

	if ( m_is_update_needed ) {

		m_is_update_needed = false;
		Log( "UPDATE" );

		for ( const auto& it : m_cache_children_by_zindex ) {
			for ( const auto& child : it.second ) {
				if ( child->m_type == AT_CACHE ) {
					( (Cache*)child )->UpdateCacheImpl( shader_program, camera );
				}
			}
		}

		{
			const auto& area = ( (scene::actor::Cache*)m_actor )->GetEffectiveArea();
			if ( area.width && area.height ) {
				m_fbo->Align(
					/*{
						(size_t)area.left,
						(size_t)area.top,
					}, {
						(size_t)area.right,
						(size_t)area.bottom,
					}*/
					{
						(size_t)area.left,
						(size_t)area.top,
					},
					{
						(size_t)area.right,
						(size_t)area.bottom,
					}
				);
				//m_fbo->Resize( m_opengl->GetViewportWidth(), m_opengl->GetViewportHeight() );
				//m_fbo->Resize( m_opengl->GetViewportWidth() - 100, m_opengl->GetViewportHeight() - 100 );
				m_fbo->Resize( area.width, area.height );
			}
		}

		m_fbo->Write(
			[ this, &shader_program, &camera ]() {
				for ( const auto& it : m_cache_children_by_zindex ) {
					for ( const auto& child : it.second ) {
						child->DrawImpl( shader_program, camera );
					}
				}
			}
		);

	}
}

void Cache::DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {
	ASSERT( shader_program->GetType() == shader_program::ShaderProgram::TYPE_SIMPLE2D, "invalid shader program type" );

	m_fbo->Draw( (shader_program::Simple2D*)shader_program );
}

void Cache::AddCacheChildToZIndexSet( Actor* gl_actor, const float zindex ) {
	auto it = m_cache_children_by_zindex.find( zindex );
	if ( it == m_cache_children_by_zindex.end() ) {
		m_cache_children_by_zindex[ zindex ] = {};
		it = m_cache_children_by_zindex.find( zindex );
	}
	ASSERT( std::find( it->second.begin(), it->second.end(), gl_actor ) == it->second.end(), "actor already found in zindex set" );
	Log( "Adding actor " + gl_actor->GetName() + " to zindex set " + std::to_string( zindex ) );
	it->second.push_back( gl_actor );
}

void Cache::RemoveCacheChildFromZIndexSet( Actor* gl_actor, const float zindex ) {
	Log( "Removing actor " + gl_actor->GetName() + " from zindex set " + std::to_string( zindex ) );
	auto it = m_cache_children_by_zindex.find( zindex );
	ASSERT( it != m_cache_children_by_zindex.end(), "zindex set not found" );
	auto actor_it = find( it->second.begin(), it->second.end(), gl_actor );
	ASSERT( actor_it != it->second.end(), "actor not found in zindex set" );
	it->second.erase( actor_it );
}

}
}
