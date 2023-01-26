#include "Scene.h"

#include "actor/Mesh.h"
#include "actor/Image.h"

#include "scene/actor/Actor.h"

#include "../Graphics.h"

namespace graphics {
namespace opengl {

Scene::Scene( scene::Scene *scene, routine::Routine *routine ) : m_scene( scene ), m_routine( routine ) {
	m_name = scene->GetLocalName();
}

Scene::~Scene() {
	for ( auto it = m_gl_actors.begin() ; it < m_gl_actors.end() ; ++it ) {
		RemoveActor( *it );
	}

}

void Scene::RemoveActor( base::ObjectLink *link ) {
	auto *gl_actor = link->GetDstObject<Actor>();
	if ( link->Removed() ) {
		// already removed on other side
		gl_actor->UnlinkActor();
	}
	else {
		// unlink from our side
		link->GetSrcObject<scene::actor::Actor>()->m_graphics_object = NULL;
	}
	gl_actor->Unload();
	DELETE( gl_actor );
	DELETE( link );
}

void Scene::AddActorToZIndexSet( Actor* gl_actor ) {
	ASSERT( m_gl_actors_by_zindex[ gl_actor->GetPosition().z ].find( gl_actor ) == m_gl_actors_by_zindex[ gl_actor->GetPosition().z ].end(),
		"actor to be added already found in zindex set"
	);
	//Log( "Adding actor " + gl_actor->GetName() + " to zindex set " + to_string( gl_actor->GetPosition().z ) );
	m_gl_actors_by_zindex[ gl_actor->GetPosition().z ].insert( gl_actor );
}

void Scene::RemoveActorFromZIndexSet( Actor* gl_actor ) {
	ASSERT( m_gl_actors_by_zindex[ gl_actor->GetPosition().z ].find( gl_actor ) != m_gl_actors_by_zindex[ gl_actor->GetPosition().z ].end(),
		"actor to be removed not found in zindex set"
	);
	//Log( "Removing actor " + gl_actor->GetName() + " from zindex set " + to_string( gl_actor->GetPosition().z ) );
	m_gl_actors_by_zindex[ gl_actor->GetPosition().z ].erase( gl_actor );
}

void Scene::Update() {
	base::ObjectLink *obj;

	// remove missing actors
	for ( auto it = m_gl_actors.begin() ; it < m_gl_actors.end() ; ++it ) {
		Actor *gl_actor = (*it)->GetDstObject<Actor>();
		if ( (*it)->Removed() ) {
			RemoveActorFromZIndexSet( gl_actor );
			RemoveActor( *it );
			m_gl_actors.erase( it, it + 1 );
			it--;
		}
		else {
			bool reload_needed = gl_actor->ReloadNeeded();
			// check if position changed
			auto& pos = gl_actor->GetActor()->GetPosition();
			if ( gl_actor->GetPosition() != pos ) {
				// move to corrent zindex set
				RemoveActorFromZIndexSet( gl_actor );
				gl_actor->SetPosition( pos );
				AddActorToZIndexSet( gl_actor );
				reload_needed = true;
			}
			if ( reload_needed ) {
				gl_actor->Unload();
				gl_actor->Load();
			}
		}
	}

	// add new actors
	auto* actors = GetScene()->GetActors();
	for ( auto it = actors->begin() ; it < actors->end() ; it++ ) {
		obj = (*it)->m_graphics_object;
		if ( obj == NULL ) {

			Actor *gl_actor = NULL;

			auto actor_type = (*it)->GetType();
			switch (actor_type) {
				case (scene::actor::Actor::TYPE_MESH):
					NEW( gl_actor, Mesh, (scene::actor::Mesh *)*it );
					break;
				case (scene::actor::Actor::TYPE_IMAGE):
					NEW( gl_actor, Image, (scene::actor::Image *)*it );
					break;
				default:
					gl_actor = m_routine->AddCustomActor( *it );
			}

			if ( gl_actor ) {
				gl_actor->Load();
				NEW( obj, base::ObjectLink, (*it), gl_actor );
				m_gl_actors.push_back( obj );
				AddActorToZIndexSet( gl_actor );
				(*it)->m_graphics_object = obj;
			}
		}
	}

#ifdef DEBUG
	size_t gl_actors_by_zindex_count = 0;
	for (auto& actors : m_gl_actors_by_zindex) {
		gl_actors_by_zindex_count += actors.second.size();
	}
	if ( gl_actors_by_zindex_count != m_gl_actors.size() ) {
		THROW( "gl_actors_by_zindex count does not match gl_actors count ( " + to_string( gl_actors_by_zindex_count ) + " , " + to_string( m_gl_actors.size() ) + " )" );
	}
#endif
}

void Scene::Draw( shader_program::ShaderProgram *shader_program ) {
	
#ifdef DEBUG
	float last_zindex = -9999999;
	string zindex_sequence = "";
#endif
	for ( auto& actors : m_gl_actors_by_zindex ) {
#ifdef DEBUG
		float zindex = actors.first;
		zindex_sequence += " " + to_string( zindex );
		if ( zindex < last_zindex ) {
			THROW( "invalid zindex sequence: " + zindex_sequence );
		}
		last_zindex = zindex;
#endif
		
		auto *camera = m_scene->GetCamera();
		if ( camera ) {

			//glEnable( GL_CULL_FACE );

			//shader_program->Enable();

			//glUniformMatrix4fv( shader_program->m_gl_uniforms.pvm, 1, GL_TRUE, (const GLfloat*)&camera->GetMatrix() );
		}
		
		for ( auto& actor : actors.second ) {
			actor->Draw( shader_program, m_scene->GetCamera() );
		}
	}
	
}

} /* namespace opengl */
} /* namespace graphics */
