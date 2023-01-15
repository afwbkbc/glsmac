#include "Scene.h"

#include "MeshActor.h"
#include "ImageActor.h"

#include "scene/actor/Actor.h"

#include "../Renderer.h"

namespace renderer {
namespace opengl {

Scene::Scene( scene::Scene *scene, routine::OpenGLRoutine *routine ) : m_scene( scene ), m_routine( routine ) {
	m_name = scene->GetLocalName();
}

Scene::~Scene() {
	for ( auto it = m_gl_actors.begin() ; it < m_gl_actors.end() ; ++it )
		RemoveActor( *it );

}

void Scene::RemoveActor( base::ObjectLink *link ) {
	auto *gl_actor = link->GetDstObject<Actor>();
	if ( link->Removed() ) {
		gl_actor->UnlinkActor(); // already removed on other side
	}
	else {
		link->GetSrcObject<scene::actor::Actor>()->m_renderer_object = NULL;
	}
	gl_actor->Unload();
	DELETE( gl_actor );
	DELETE( link );
}

void Scene::Update() {
	base::ObjectLink *obj;

	// remove missing actors
	for ( auto it = m_gl_actors.begin() ; it < m_gl_actors.end() ; ++it ) {
		if ( (*it)->Removed() ) {
			RemoveActor( *it );
			m_gl_actors.erase( it, it + 1 );
			it--;
		}
		else {
			Actor *gl_actor = (*it)->GetDstObject<Actor>();
			if ( gl_actor->ReloadNeeded() ) {
				gl_actor->Unload();
				gl_actor->Load();
			}
		}
	}

	// add new actors
	auto* actors = GetScene()->GetActors();
	for ( auto it = actors->begin() ; it < actors->end() ; it++ ) {
		obj = (*it)->m_renderer_object;
		if ( obj == NULL ) {

			Actor *gl_actor = NULL;

			auto actor_type = (*it)->GetType();
			switch (actor_type) {
				case (scene::actor::Actor::TYPE_MESH):
					NEW( gl_actor, MeshActor, (scene::actor::MeshActor *)*it );
					break;
				case (scene::actor::Actor::TYPE_IMAGE):
					NEW( gl_actor, ImageActor, (scene::actor::ImageActor *)*it );
					break;
				default:
					gl_actor = m_routine->AddCustomActor( *it );
			}

			if ( gl_actor ) {
				gl_actor->Load();
				NEW( obj, base::ObjectLink, (*it), gl_actor );
				m_gl_actors.push_back( obj );
				(*it)->m_renderer_object = obj;
			}
		}
	}

}

void Scene::Draw( shader_program::OpenGLShaderProgram *shader_program ) {
	for ( auto it = m_gl_actors.begin() ; it < m_gl_actors.end() ; ++it ) {
		(*it)->GetDstObject<Actor>()->Draw( shader_program );
	}
}

} /* namespace opengl */
} /* namespace renderer */
