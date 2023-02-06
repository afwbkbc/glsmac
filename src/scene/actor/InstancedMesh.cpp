#include "InstancedMesh.h"

namespace scene {
namespace actor {

InstancedMesh::InstancedMesh( const std::string &name, const types::Mesh *mesh ) : Mesh( name, mesh, TYPE_INSTANCED_MESH ) {
	
}

InstancedMesh::~InstancedMesh() {
	
}

const InstancedMesh::world_matrices_t& InstancedMesh::GetWorldMatrices() {
	if ( m_need_world_matrix_update ) {
		UpdateWorldMatrix();
	}
	return m_world_matrices;
}

types::Matrix44& InstancedMesh::GetWorldMatrix() {
	ASSERT( false, "use GetWorldMatrices() for instanced mesh" );
}
	
void InstancedMesh::UpdateWorldMatrix() {
	if ( m_scene ) {
		auto* camera = m_scene->GetCamera();
		if ( camera ) {
			auto& instances = m_scene->GetInstances();
			m_world_matrices.resize( instances.size() );
			for ( auto i = 0 ; i < instances.size() ; i++ ) {
				m_world_matrices[i] = camera->GetMatrix() * m_instance_matrices[i].matrix;
			}
			m_need_world_matrix_update = false;
		}
	}
}

void InstancedMesh::UpdatePosition() {
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

void InstancedMesh::UpdateMatrix() {
	auto instances = GetInstances();
	if ( instances ) {
		for ( auto i = 0 ; i < instances->size() ; i++ ) {
			m_instance_matrices[i].matrix = m_instance_matrices[i].translate * m_matrices.rotate * m_matrices.scale;
		}
	}
	m_need_world_matrix_update = true;
}

const scene::Scene::instances_t* InstancedMesh::GetInstances() {
	if ( m_scene ) {
		auto* instances = &m_scene->GetInstances();
		if ( m_instance_matrices.size() != instances->size() ) {
			m_instance_matrices.resize( instances->size() );
		}
		return instances;
	}
	return nullptr;
}

}
}
