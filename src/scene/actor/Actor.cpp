#include "Actor.h"

#include "../Scene.h"

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

void Actor::SetAreaLimits( const area_limits_t limits ) {
	//Log( "Setting area limits to " + limits.first.ToString() + "," + limits.second.ToString() );
	m_render_flags |= RF_USE_AREA_LIMITS;
	m_area_limits = limits;
}

void Actor::RemoveAreaLimits() {
	//Log( "Removing area limits" );
	m_render_flags &=~ RF_USE_AREA_LIMITS;
}

const Actor::area_limits_t& Actor::GetAreaLimits() const {
	return m_area_limits;
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
