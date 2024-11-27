#include "Scene.h"

#include <algorithm>

#include "actor/Sprite.h"
#include "actor/Mesh.h"
#include "common/ObjectLink.h"
#include "scene/Scene.h"
#include "scene/actor/Actor.h"
#include "scene/actor/Mesh.h"
#include "scene/actor/Sprite.h"
#include "scene/actor/Instanced.h"
#include "routine/Routine.h"
#include "texture/Texture.h"

namespace graphics {
namespace opengl {

Scene::Scene( OpenGL* opengl, scene::Scene* scene, routine::Routine* routine )
	: m_opengl( opengl )
	, m_scene( scene )
	, m_routine( routine ) {
	m_name = scene->GetLocalName();
}

Scene::~Scene() {
	for ( auto it = m_gl_actors.begin() ; it < m_gl_actors.end() ; ++it ) {
		RemoveActor( *it );
	}

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
	common::ObjectLink* obj;

	for ( auto it = m_gl_actors.begin() ; it < m_gl_actors.end() ; ++it ) {
		Actor* gl_actor = ( *it )->GetDstObject< Actor >();
		if ( ( *it )->Removed() ) {
			// remove missing actors

			RemoveActorFromZIndexSet( gl_actor );
			RemoveActor( *it );
			m_gl_actors.erase( it, it + 1 );
			it--;
		}
		else {
			// reload actors when needed

			bool mesh_reload_needed = gl_actor->MeshReloadNeeded();
			bool texture_reload_needed = gl_actor->TextureReloadNeeded();

			float z_index = 0.0f;
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

	// add new actors
	auto* actors = GetScene()->GetActors();
	for ( auto it = actors->begin() ; it < actors->end() ; it++ ) {
		obj = ( *it )->m_graphics_object;
		if ( obj == NULL ) {

			Actor* gl_actor = NULL;

			auto actor_type = ( *it )->GetType();
			switch ( actor_type ) {
				case scene::actor::Actor::TYPE_SPRITE:
				case scene::actor::Actor::TYPE_INSTANCED_SPRITE: {
					NEW( gl_actor, Sprite, m_opengl, (scene::actor::Sprite*)*it );
					break;
				}
				case scene::actor::Actor::TYPE_MESH:
				case scene::actor::Actor::TYPE_INSTANCED_MESH: {
					NEW( gl_actor, Mesh, m_opengl, (scene::actor::Mesh*)*it );
					break;
				}
				default: {
					gl_actor = m_routine->AddCustomActor( *it );
				}
			}

			if ( gl_actor ) {
				gl_actor->LoadMesh();
				gl_actor->LoadTexture();
				NEW( obj, common::ObjectLink, ( *it ), gl_actor );
				m_gl_actors.push_back( obj );
				AddActorToZIndexSet( gl_actor ); // TODO: only Simple2D
				( *it )->m_graphics_object = obj;
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
Texture* Scene::GetSkyboxTexture() const {
	if ( ( !m_skybox_texture ) || m_skybox_texture->Removed() ) {
		return NULL;
	}
	return m_skybox_texture->GetDstObject< Texture >();
}
common::ObjectLink* Scene::GetSkyboxTextureObj() const {
	return m_skybox_texture;
}
void Scene::SetSkyboxTextureObj( common::ObjectLink* skybox_texture ) {
	m_skybox_texture = skybox_texture;
}

void Scene::Draw( shader_program::ShaderProgram* shader_program, shader_program::ShaderProgram* other_shader_program ) {

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
				// TODO: refactor
				if ( actor->GetActor()->GetType() == scene::actor::Actor::TYPE_TEXT ) {
					ASSERT( other_shader_program, "text actor needs other_shader_program but it's null" );
					actor->Draw( other_shader_program, m_scene->GetCamera() );
				}
				else {
					actor->Draw( shader_program, m_scene->GetCamera() );
				}
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
