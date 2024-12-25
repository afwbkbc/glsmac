#include "Cache.h"

#include "engine/Engine.h"
#include "graphics/opengl/OpenGL.h"
#include "graphics/opengl/shader_program/Simple2D.h"
#include "scene/actor/Cache.h"

namespace graphics {
namespace opengl {

Cache::Cache( OpenGL* opengl, scene::actor::Actor* actor )
	: Actor( opengl, actor ) {
	//Log( "Creating OpenGL text '" + actor->GetText() + "' with font " + font->m_name );
	//glGenBuffers( 1, &m_vbo );
}

Cache::~Cache() {
	if ( !m_cache_children.empty() ) {
		Log( "WARNING: cache children not empty" );
		for ( const auto& c : m_cache_children ) {
			Log( c->GetName() );
		}
	}
	//Log( "Destroying OpenGL text" );
	//glDeleteBuffers( 1, &m_vbo );
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

void Cache::DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {
	ASSERT( shader_program->GetType() == shader_program::ShaderProgram::TYPE_SIMPLE2D, "unexpected shader program" );

	// TODO: cache

	for ( const auto& it : m_cache_children_by_zindex ) {
		for ( const auto& child : it.second ) {
			child->DrawImpl( shader_program, camera );
		}
	}

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
