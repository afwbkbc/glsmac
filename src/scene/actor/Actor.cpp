#include "graphics/Graphics.h"

#include "Actor.h"

namespace scene {
namespace actor {

Actor::Actor( type_t type, const std::string &name ) : Entity(), m_type( type ) {
	m_name = name;
}

Actor::~Actor() {
	if ( m_graphics_object ) {
		m_graphics_object->Remove();
	}
}

types::Matrix44 & Actor::GetWorldMatrix() {
	if ( m_need_world_matrix_update )
		UpdateWorldMatrix();
	return m_actor_matrices.world;
}

void Actor::UpdateWorldMatrix() {
	if ( m_scene ) {
		auto *camera = m_scene->GetCamera();
		if ( camera ) {
			m_actor_matrices.world = camera->GetMatrix() * GetMatrix();
			m_need_world_matrix_update = false;
		}
	}
}

void Actor::UpdateMatrix() {
	Entity::UpdateMatrix();
	
	m_need_world_matrix_update = true;
}

void Actor::SetScene( Scene *scene ) {
	ASSERT( m_scene == NULL || scene == NULL, "scene overlap" );
	m_scene = scene;
}

Scene *Actor::GetScene() {
	ASSERT( m_scene, "scene not set" );
	return m_scene;
}

void Actor::SetCoordinateLimits( const coordinate_limits_t limits ) {
	//Log( "Setting coordinate limits to " + limits.first.ToString() + "," + limits.second.ToString() );
	m_render_flags |= RF_USE_COORDINATE_LIMITS;
	m_coordinate_limits = limits;
}

const Actor::coordinate_limits_t& Actor::GetCoordinateLimits() const {
	return m_coordinate_limits;
}

void Actor::SetRenderFlags( const render_flag_t render_flags ) {
	m_render_flags = render_flags;
}

const Actor::render_flag_t Actor::GetRenderFlags() const {
	return m_render_flags;
}

const types::Buffer Actor::Serialize() const {
	types::Buffer buf = Entity::Serialize();
	
	buf.WriteInt( m_type );
	
	return buf;
}

void Actor::Unserialize( types::Buffer buf ) {
	Entity::Unserialize( buf );
	// HACK! TODO: refactor
	buf.ReadVec3();
	buf.ReadVec3();
	
	type_t type = (type_t)buf.ReadInt();
	ASSERT( type == m_type, "loaded actor of wrong type ( " + std::to_string( type ) + " != " + std::to_string( m_type ) + " )" );
	
	UpdateMatrix();
}


} /* namespace actor */
} /* namespace scene */
