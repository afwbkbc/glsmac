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

#define _XYZ_ASSERT() ASSERT( false, "use AddInstance()/SetInstance()/RemoveInstance() for instanced actors" )
#define _XYZ_SETTER( _name, _var, _updates ) \
const types::Vec3& Instanced::Get##_name() const {\
	_XYZ_ASSERT(); \
	return m_##_var;\
}\
const float Instanced::Get##_name##X() const {\
	_XYZ_ASSERT(); \
	return 0;\
}\
const float Instanced::Get##_name##Y() const {\
	_XYZ_ASSERT(); \
	return 0;\
}\
const float Instanced::Get##_name##Z() const {\
	_XYZ_ASSERT(); \
	return 0;\
}\
void Instanced::Set##_name( const types::Vec3 & value ) {\
	_XYZ_ASSERT(); \
}\
void Instanced::Set##_name##X( const float value ) {\
	_XYZ_ASSERT(); \
}\
void Instanced::Set##_name##Y( const float value ) {\
	_XYZ_ASSERT(); \
}\
void Instanced::Set##_name##Z( const float value ) {\
	_XYZ_ASSERT(); \
}

_XYZ_SETTER( Angle, angle, UpdateRotation(); )
_XYZ_SETTER( Position, position, UpdatePosition(); )

#undef _XYZ_SETTER
#undef _XYZ_ASSERT

const Instanced::matrices_t& Instanced::GetWorldMatrices() {
	if ( m_need_world_matrix_update ) {
		//Log( "Updating world matrices" );
		UpdateWorldMatrix();
	}
	return m_world_matrices;
}

types::Matrix44& Instanced::GetWorldMatrix() {
	ASSERT( false, "use GetWorldMatrices() for instanced actors" );
	return m_actor_matrices.world; // just to fix warning
}

void Instanced::GenerateWorldMatrices( matrices_t* out_matrices, scene::Camera* camera ) {
	const auto& world_instance_positions = m_scene->GetWorldInstancePositions();
	out_matrices->resize( world_instance_positions.size() * m_instances.size() );
	size_t i = 0;
	
	// SPAMMY
	//Log( "Updating " + std::to_string( m_instances.size() ) + " instances" );
	
	for ( auto& id_instance : m_instances ) {
		auto& instance = id_instance.second;
		if ( instance.need_update ) {
			UpdateInstance( instance );
		}
		for ( auto& matrices : instance.matrices ) {
			(*out_matrices)[ i ] = camera->GetMatrix() * matrices.matrix;
			i++;
		}
	}
}

void Instanced::UpdateWorldMatrix() {
	if ( m_scene ) {
		auto* camera = m_scene->GetCamera();
		if ( camera ) {
			GenerateWorldMatrices( &m_world_matrices, camera );
			if ( IS_TESTING( this ) ) {
				//Log( "WORLD MATRICES: " );
				for ( auto& m : m_world_matrices ) {
					Log( "  " + m.ToString() );
				}
			}
			m_need_world_matrix_update = false;
		}
	}
}

void Instanced::UpdatePosition() {
	UpdateMatrix();
}

void Instanced::UpdateMatrix() {
	for ( auto& instance : m_instances ) {
		instance.second.need_update = true;
	}
	m_need_world_matrix_update = true;
}

const scene::Scene::instance_positions_t* Instanced::GetWorldInstancePositions() {
	if ( m_scene ) {
		return &m_scene->GetWorldInstancePositions();
	}
	return nullptr;
}

void Instanced::UpdateInstance( instance_t& instance ) {
	const auto& world_instance_positions = GetWorldInstancePositions();
	const size_t sz = world_instance_positions->size();
	instance.matrices.resize( world_instance_positions->size() );
	//Log( "Updating for " + std::to_string( world_instance_positions->size() ) + " world instances" );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		auto& matrices = instance.matrices[ i ];
		auto& world_position = (*world_instance_positions)[ i ];
		matrices.translate.TransformTranslate(
			instance.position.x + world_position.x,
			instance.position.y + world_position.y,
			instance.position.z + world_position.z
		);
		matrices.matrix =
			matrices.translate *
			m_matrices.rotate *
			m_matrices.scale
		; // TODO: per-instance rotate and scale too
	}
	
	instance.need_update = false;
}

Sprite* Instanced::GetSpriteActor() const {
	ASSERT( m_type == TYPE_INSTANCED_SPRITE, "GetSpriteActor on non-sprite actor" );
	return (Sprite*)m_actor;
}

Mesh* Instanced::GetMeshActor() const {
	ASSERT( m_type == TYPE_INSTANCED_MESH, "GetMeshActor on non-mesh actor" );
	return (Mesh*)m_actor;
}

const Instanced::instance_id_t Instanced::AddInstance( const types::Vec3& position, const types::Vec3& angle ) {
	m_instances[ m_next_instance_id ] = { position, angle, {}, true };
	m_need_world_matrix_update = true;
	return m_next_instance_id++;
}

void Instanced::SetInstance( const instance_id_t instance_id, const types::Vec3& position, const types::Vec3& angle ) {
	ASSERT( m_instances.find( instance_id ) != m_instances.end(), "instance " + std::to_string( instance_id ) + " not found" );
	m_need_world_matrix_update = true;
	m_instances[ instance_id ] = { position, angle, {}, true };
}

void Instanced::RemoveInstance( const instance_id_t instance_id ) {
	ASSERT( m_instances.find( instance_id ) != m_instances.end(), "instance " + std::to_string( instance_id ) + " not found" );
	m_need_world_matrix_update = true;
	m_instances.erase( instance_id );
}

const float Instanced::GetZIndex() const {
	return m_z_index;
}

void Instanced::SetZIndex( const float z_index ) {
	m_z_index = z_index;
}

}
}
