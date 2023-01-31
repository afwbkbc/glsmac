#include <algorithm>

#include "Scene.h"

namespace scene {

Scene::~Scene() {
	if ( m_local_camera ) {
		DELETE( m_local_camera );
	}
}

void Scene::AddActor( actor::Actor *actor ) {
	//Log( "Adding actor [" + actor->GetName() + "]" );
	actor->SetScene( this );
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

void Scene::SetLight( Light *light ) {
	ASSERT( m_light == NULL || light == NULL, "light overlap" );
	m_light = light;
}

Light* Scene::GetLight() const {
	return m_light;
}


void Scene::SetSkyboxTexture( types::Texture *skybox_texture ) {
	m_skybox_texture = skybox_texture;
}

types::Texture *Scene::GetSkyboxTexture() {
	return m_skybox_texture;
}


} /* namespace scene */
