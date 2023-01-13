#include <algorithm>

#include "Scene.h"

namespace scene {

void Scene::AddActor( actor::Actor *actor ) {
	Log( "Adding actor [" + actor->GetName() + "]" );
	actor->SetScene( this );
	m_actors.push_back( actor );
}

void Scene::RemoveActor( actor::Actor *actor ) {
	auto it = std::find( m_actors.begin(), m_actors.end(), actor );
	if ( it < m_actors.end() ) {
		Log( "Removing actor [" + actor->GetName() + "]" );
		actor->SetScene( NULL );
		m_actors.erase( it, it + 1 );
	}
}

void Scene::SetCamera( Camera *camera ) {
	if ( m_camera != NULL && camera != NULL )
		throw SceneError( "duplicate camera set" );
	camera->SetScene( this );
	m_camera = camera;
}

Camera * Scene::GetCamera() {
	return m_camera;
}

void Scene::SetSkyboxTexture( types::Texture *skybox_texture ) {
	m_skybox_texture = skybox_texture;
}

types::Texture *Scene::GetSkyboxTexture() {
	return m_skybox_texture;
}


} /* namespace scene */
