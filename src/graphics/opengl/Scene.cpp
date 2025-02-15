#include "Scene.h"

#include <algorithm>

#include "actor/Sprite.h"
#include "actor/Mesh.h"
#include "actor/Text.h"
#include "actor/Cache.h"
#include "common/ObjectLink.h"
#include "scene/Scene.h"
#include "scene/actor/Actor.h"
#include "scene/actor/Mesh.h"
#include "scene/actor/Sprite.h"
#include "scene/actor/Instanced.h"
#include "scene/actor/Text.h"
#include "scene/actor/Cache.h"
#include "routine/Routine.h"

namespace graphics {
namespace opengl {

Scene::Scene( OpenGL* opengl, scene::Scene* scene, routine::Routine* routine )
	: m_opengl( opengl )
	, m_scene( scene )
	, m_routine( routine ) {
	m_name = scene->GetLocalName();
}

Scene::~Scene() {
	for ( auto it = m_gl_actors.rbegin() ; it < m_gl_actors.rend() ; ++it ) {
		RemoveActor( *it );
	}

}

Actor* Scene::CreateActor( scene::actor::Actor* const actor ) const {
	Actor* gl_actor = nullptr;
	switch ( actor->GetType() ) {
		case scene::actor::Actor::TYPE_SPRITE:
		case scene::actor::Actor::TYPE_INSTANCED_SPRITE: {
			NEW( gl_actor, Sprite, m_opengl, (scene::actor::Sprite*)actor );
			break;
		}
		case scene::actor::Actor::TYPE_MESH:
		case scene::actor::Actor::TYPE_INSTANCED_MESH: {
			NEW( gl_actor, Mesh, m_opengl, (scene::actor::Mesh*)actor );
			break;
		}
		case scene::actor::Actor::TYPE_TEXT: {
			auto* text_actor = (scene::actor::Text*)actor;
			NEW( gl_actor, Text, m_opengl, text_actor, text_actor->GetFont() );
			break;
		}
		case scene::actor::Actor::TYPE_CACHE: {
			auto* cache_actor = (scene::actor::Cache*)actor;
			NEW( gl_actor, Cache, m_opengl, cache_actor );
			break;
		}
		default: {
			gl_actor = m_routine->AddCustomActor( actor );
		}
	}

	if ( gl_actor ) {
		gl_actor->LoadMesh();
		gl_actor->LoadTexture();
		auto* cache_parent = actor->GetCacheParent();
		if ( cache_parent ) {
			ASSERT( cache_parent->m_graphics_object && cache_parent->m_graphics_object, "cache parent has no graphics object link" );
			auto* gl_cache_parent = cache_parent->m_graphics_object->GetDstObject< Cache >();
			ASSERT( gl_cache_parent, "gl cache parent not set" );
			gl_actor->SetCacheParent( gl_cache_parent );
		}
	}

	return gl_actor;
}

void Scene::RemoveActor( common::ObjectLink* link ) {
	auto* gl_actor = link->GetDstObject< Actor >();
	if ( link->Removed() ) {
		// already removed on other side
		gl_actor->UnlinkActor();
	}
	else {
		// unlink from our side
		link->GetSrcObject< scene::actor::Actor >()->m_graphics_object = NULL;
	}
	gl_actor->UnloadMesh();
	gl_actor->UnloadTexture();
	DELETE( gl_actor );
	DELETE( link );
}

void Scene::AddActorToZIndexSet( Actor* gl_actor ) {
	const float zindex = gl_actor->GetZIndex();
	auto it = m_gl_actors_by_zindex.find( zindex );
	if ( it == m_gl_actors_by_zindex.end() ) {
		m_gl_actors_by_zindex[ zindex ] = {};
		it = m_gl_actors_by_zindex.find( zindex );
	}
	ASSERT( std::find( it->second.begin(), it->second.end(), gl_actor ) == it->second.end(), "actor already found in zindex set" );
	//Log( "Adding actor " + gl_actor->GetName() + " to zindex set " + std::to_string( gl_actor->GetPosition().z ) );
	it->second.push_back( gl_actor );
}

void Scene::RemoveActorFromZIndexSet( Actor* gl_actor ) {
	const float zindex = gl_actor->GetZIndex();
	auto it = m_gl_actors_by_zindex.find( zindex );
	ASSERT( it != m_gl_actors_by_zindex.end(), "zindex set not found" );
	auto actor_it = find( it->second.begin(), it->second.end(), gl_actor );
	ASSERT( actor_it != it->second.end(), "actor not found in zindex set" );
	//Log( "Removing actor " + gl_actor->GetName() + " from zindex set " + to_string( gl_actor->GetPosition().z ) );
	it->second.erase( actor_it );
}

void Scene::Update() {

	std::vector< common::ObjectLink* > actors_to_remove = {};

	for ( auto it = m_gl_actors.begin() ; it < m_gl_actors.end() ; ++it ) {
		if ( ( *it )->Removed() ) {
			actors_to_remove.push_back( *it );
			m_gl_actors.erase( it-- );
		}
		else {

			Actor* gl_actor = ( *it )->GetDstObject< Actor >();

			// reload actors when needed
			bool mesh_reload_needed = gl_actor->MeshReloadNeeded();
			bool texture_reload_needed = gl_actor->TextureReloadNeeded();

			float z_index = 0.5f;
			const auto* actor = gl_actor->GetActor();
			if (
				actor->GetType() != scene::actor::Actor::TYPE_INSTANCED_MESH &&
					actor->GetType() != scene::actor::Actor::TYPE_INSTANCED_SPRITE
				) {
				// check if z index changed ( doesn't make sense for instanced actors )
				z_index = actor->GetPosition().z;
			}
			else {
				z_index = ( (scene::actor::Instanced*)actor )->GetZIndex();
			}
			if ( gl_actor->GetZIndex() != z_index ) {
				// move to current zindex set
				RemoveActorFromZIndexSet( gl_actor );
				gl_actor->SetZIndex( z_index );
				AddActorToZIndexSet( gl_actor );
			}

			if ( mesh_reload_needed ) {
				gl_actor->UnloadMesh();
				gl_actor->LoadMesh();
			}
			if ( texture_reload_needed ) {
				gl_actor->UnloadTexture();
				gl_actor->LoadTexture();
			}
		}
	}

	// remove missing actors
	for ( const auto& o : actors_to_remove ) {
		int a = 5;
	}

	for ( auto it = actors_to_remove.rbegin() ; it != actors_to_remove.rend() ; it++ ) {
		auto& o = *it;
		Actor* gl_actor = o->GetDstObject< Actor >();
		RemoveActorFromZIndexSet( gl_actor );
		RemoveActor( *it );
	}

	// add new actors
	common::ObjectLink* obj;

	auto* actors = GetScene()->GetActors();
	for ( auto it = actors->begin() ; it < actors->end() ; it++ ) {
		obj = ( *it )->m_graphics_object;
		if ( obj == NULL ) {

			auto* gl_actor = CreateActor( *it );

			if ( gl_actor ) {
				NEWV( obj, common::ObjectLink, *it, gl_actor );
				( *it )->m_graphics_object = obj;
				m_gl_actors.push_back( obj );
				AddActorToZIndexSet( gl_actor ); // TODO: only Simple2D
			}

		}
	}

#ifdef DEBUG
	size_t gl_actors_by_zindex_count = 0;
	for ( auto& actors : m_gl_actors_by_zindex ) {
		gl_actors_by_zindex_count += actors.second.size();
	}
	if ( gl_actors_by_zindex_count != m_gl_actors.size() ) {
		THROW( "gl_actors_by_zindex count does not match gl_actors count ( " + std::to_string( gl_actors_by_zindex_count ) + " , " + std::to_string( m_gl_actors.size() ) + " )" );
	}
#endif
}

scene::Scene* Scene::GetScene() const {
	return m_scene;
}

void Scene::Draw( shader_program::ShaderProgram* shader_program ) {

#ifdef DEBUG
	float last_zindex = -9999999;
	std::string zindex_sequence = "";
#endif
	for ( auto& actors : m_gl_actors_by_zindex ) {
#ifdef DEBUG
		float zindex = actors.first;
		zindex_sequence += " " + std::to_string( zindex );
		if ( zindex < last_zindex ) {
			THROW( "invalid zindex sequence: " + zindex_sequence );
		}
		last_zindex = zindex;
#endif

		for ( auto& actor : actors.second ) {
			if ( actor->GetActor()->IsVisible() ) {
				actor->Draw( shader_program, m_scene->GetCamera() );
			}
		}
	}

}

void Scene::OnWindowResize() {
	for ( auto& link : m_gl_actors ) {
		auto* gl_actor = link->GetDstObject< Actor >();
		gl_actor->OnWindowResize();
	}
}

}
}
