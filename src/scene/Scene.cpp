#include <algorithm>

#include "Scene.h"
#include "graphics/Graphics.h"

namespace scene {

Scene::~Scene() {
	if ( m_local_camera ) {
		DELETE( m_local_camera );
	}
}

void Scene::AddActor( actor::Actor *actor ) {
	//Log( "Adding actor [" + actor->GetName() + "]" );
	actor->SetScene( this );
	actor->UpdatePosition();
	m_actors.push_back( actor );
}

void Scene::RemoveActor( actor::Actor *actor ) {
	auto it = std::find( m_actors.begin(), m_actors.end(), actor );
	if ( it < m_actors.end() ) {
		//Log( "Removing actor [" + actor->GetName() + "]" );
		actor->SetScene( NULL );
		m_actors.erase( it, it + 1 );
	}
}

void Scene::SetCamera( Camera *camera ) {
	ASSERT( m_camera == NULL || camera == NULL, "camera overlap" );
	camera->SetScene( this );
	m_camera = camera;
}

Camera * Scene::GetCamera() const {
	return m_camera;
}

void Scene::AddLight( Light *light ) {
	ASSERT( m_lights.find( light ) == m_lights.end(), "light overlap" );
	ASSERT( m_lights.size() < graphics::Graphics::MAX_WORLD_LIGHTS, "maximum light count exceeded" );
	m_lights.insert( light );
}

std::unordered_set< Light* >* Scene::GetLights() {
	return &m_lights;
}

void Scene::SetSkyboxTexture( types::Texture *skybox_texture ) {
	m_skybox_texture = skybox_texture;
}

types::Texture *Scene::GetSkyboxTexture() {
	return m_skybox_texture;
}

void Scene::SetInstances( const instances_t& instances ) {
	Log( "Setting " + std::to_string( instances.size() ) + " instances" );
	m_instances = instances;
	for ( auto& actor : m_actors ) {
		actor->UpdatePosition();
	}
}

const Scene::instances_t& Scene::GetInstances() const {
	return m_instances;
}


} /* namespace scene */
