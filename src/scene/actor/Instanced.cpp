#include "Instanced.h"

namespace scene {
namespace actor {

Instanced::Instanced( Actor* actor )
	: Actor(
		(type_t)( (uint8_t)actor->GetType() + 1 ), // assume normal type is always followed by instanced type
		"Instanced" + actor->GetLocalName()
	),
	m_actor( actor )
{
	//
}

Instanced::~Instanced() {
	DELETE( m_actor );
}

const Instanced::world_matrices_t& Instanced::GetWorldMatrices() {
	if ( m_need_world_matrix_update ) {
		UpdateWorldMatrix();
	}
	return m_world_matrices;
}

types::Matrix44& Instanced::GetWorldMatrix() {
	ASSERT( false, "use GetWorldMatrices() for instanced actors" );
	return m_actor_matrices.world; // just to fix warning
}

Instanced::world_matrices_t Instanced::GenerateWorldMatrices( scene::Camera* camera ) {
	world_matrices_t matrices;
	auto& instances = m_scene->GetInstances();
	matrices.resize( instances.size() );
	for ( auto i = 0 ; i < instances.size() ; i++ ) {
		matrices[i] = camera->GetMatrix() * m_instance_matrices[i].matrix;
	}
	return matrices;
}

void Instanced::UpdateWorldMatrix() {
	if ( m_scene ) {
		auto* camera = m_scene->GetCamera();
		if ( camera ) {
			m_world_matrices = GenerateWorldMatrices( camera );
			m_need_world_matrix_update = false;
		}
	}
}

void Instanced::UpdatePosition() {
	auto instances = GetInstances();
	if ( instances ) {
		for ( auto i = 0 ; i < instances->size() ; i++ ) {
			m_instance_matrices[i].translate.TransformTranslate(
				m_position.x + (*instances)[i].x,
				m_position.y + (*instances)[i].y,
				m_position.z + (*instances)[i].z
			);
		}
		UpdateMatrix();
	}
}

void Instanced::UpdateMatrix() {
	auto instances = GetInstances();
	if ( instances ) {
		for ( auto i = 0 ; i < instances->size() ; i++ ) {
			m_instance_matrices[i].matrix = m_instance_matrices[i].translate * m_matrices.rotate * m_matrices.scale;
		}
	}
	m_need_world_matrix_update = true;
}

const scene::Scene::instances_t* Instanced::GetInstances() {
	if ( m_scene ) {
		auto* instances = &m_scene->GetInstances();
		if ( m_instance_matrices.size() != instances->size() ) {
			m_instance_matrices.resize( instances->size() );
		}
		return instances;
	}
	return nullptr;
}

Sprite* Instanced::GetSpriteActor() const {
	ASSERT( m_type == TYPE_INSTANCED_SPRITE, "GetSpriteActor on non-sprite actor" );
	return (Sprite*)m_actor;
}

Mesh* Instanced::GetMeshActor() const {
	ASSERT( m_type == TYPE_INSTANCED_MESH, "GetMeshActor on non-mesh actor" );
	return (Mesh*)m_actor;
}

}
}
