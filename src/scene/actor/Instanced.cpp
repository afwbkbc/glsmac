#include "Instanced.h"

namespace scene {
namespace actor {

Instanced::Instanced( Actor* actor )
	: Actor(
	( type_t )( (uint8_t)actor->GetType() + 1 ), // assume normal type is always followed by instanced type
	"Instanced" + actor->GetLocalName()
)
	, m_actor( actor ) {
	//
}

Instanced::~Instanced() {
	DELETE( m_actor );
}

#define _XYZ_ASSERT() THROW( "use AddInstance()/SetInstance()/RemoveInstance() for instanced actors" )
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

const Instanced::matrices_t& Instanced::GetInstanceMatrices() {
	if ( m_need_world_matrix_update ) {
		//Log( "Updating world matrices" );
		UpdateWorldMatrix();
	}
	return m_instance_matrices;
}

types::Matrix44& Instanced::GetWorldMatrix() {
	THROW( "use GetInstanceMatrices() for instanced actors" );
	return m_actor_matrices.world; // just to fix warning
}

void Instanced::GenerateInstanceMatrices( matrices_t* out_matrices, scene::Camera* camera ) {
	const auto& world_instance_positions = m_scene->GetWorldInstancePositions();
	out_matrices->resize( world_instance_positions.size() * m_instances.size() );
	size_t i = 0;

	// SPAMMY
	//Log( "Updating " + std::to_string( m_instances.size() ) + " instances" );

	for ( auto& id_instance : m_instances ) {
		auto& instance = id_instance.second;
		if ( instance.need_update ) {
			UpdateMatrixForInstance( instance );
		}
		for ( auto& matrices : instance.matrices ) {
			( *out_matrices )[ i ] = matrices.matrix;
			i++;
		}
	}
}

void Instanced::UpdateWorldMatrix() {
	if ( m_scene && m_need_world_matrix_update ) {
		auto* camera = m_scene->GetCamera();
		if ( camera ) {
			GenerateInstanceMatrices( &m_instance_matrices, camera );
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

void Instanced::UpdateMatrixForInstance( instance_t& instance ) {
	const auto& world_instance_positions = GetWorldInstancePositions();
	const size_t sz = world_instance_positions->size();
	instance.matrices.resize( world_instance_positions->size() );
	//Log( "Updating for " + std::to_string( world_instance_positions->size() ) + " world instances" );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		auto& matrices = instance.matrices[ i ];
		auto& world_position = ( *world_instance_positions )[ i ];
		matrices.translate.TransformTranslate(
			instance.position.x + world_position.x,
			instance.position.y + world_position.y,
			instance.position.z + world_position.z
		);
		matrices.matrix =
			matrices.translate *
				m_matrices.rotate *
				m_matrices.scale; // TODO: per-instance rotate and scale too
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
	m_instances[ m_next_instance_id ] = {
		position,
		angle,
		{},
		true
	};
	m_need_world_matrix_update = true;
	return m_next_instance_id++;
}

void Instanced::SetInstance( const instance_id_t instance_id, const types::Vec3& position, const types::Vec3& angle ) {
	m_need_world_matrix_update = true;
	m_instances[ instance_id ] = {
		position,
		angle,
		{},
		true
	};
	if ( m_next_instance_id <= instance_id ) {
		m_next_instance_id = instance_id + 1;
	}
}

void Instanced::UpdateInstance( const instance_id_t instance_id, const types::Vec3& position, const types::Vec3& angle ) {
	if ( HasInstance( instance_id ) ) {
		SetInstance( instance_id, position, angle );
	}
}

void Instanced::RemoveInstance( const instance_id_t instance_id ) {
	const auto& it = m_instances.find( instance_id );
	if ( it != m_instances.end() ) {
		m_need_world_matrix_update = true;
		m_instances.erase( it );
	}
}

const bool Instanced::HasInstance( const instance_id_t instance_id ) {
	return m_instances.find( instance_id ) != m_instances.end();
}

const float Instanced::GetZIndex() const {
	return m_z_index;
}

void Instanced::SetZIndex( const float z_index ) {
	m_z_index = z_index;
}

const types::Buffer Instanced::Serialize() const {
	auto buf = Actor::Serialize();

	buf.WriteFloat( m_z_index );

	buf.WriteInt( m_instances.size() );
	for ( auto& id_instance : m_instances ) {
		buf.WriteInt( id_instance.first );
		const auto& instance = id_instance.second;
		buf.WriteVec3( instance.position );
		buf.WriteVec3( instance.angle );
	}

	buf.WriteInt( m_next_instance_id );

	buf.WriteString( m_actor->Serialize().ToString() );

	return buf;
}

void Instanced::Unserialize( types::Buffer buf ) {
	Actor::Unserialize( buf );

	// HACK! TODO: refactor
	buf.ReadVec3();
	buf.ReadVec3();
	buf.ReadInt();

	m_z_index = buf.ReadFloat();

	const size_t count = buf.ReadInt();
	m_instances.clear();
	for ( size_t i = 0 ; i < count ; i++ ) {
		const auto id = buf.ReadInt();
		m_instances[ id ] = {
			buf.ReadVec3(),
			buf.ReadVec3(),
			{},
			true
		};
	}

	m_next_instance_id = buf.ReadInt();

	m_actor->Unserialize( buf.ReadString() );

	m_need_world_matrix_update = true;
}

}
}
