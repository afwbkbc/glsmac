#include "renderer/Renderer.h"

#include "Actor.h"

namespace scene {
namespace actor {

Actor::Actor( type_t type, const std::string &name ) : Entity(), m_type( type ) {
	m_name = name;
}

Actor::~Actor() {
	if ( m_renderer_object )
		m_renderer_object->Remove();
}
/*
void Actor::SetPosition( const vec3_t position ) {
	m_position = position;

	UpdatePosition();
}

Actor::vec3_t Actor::GetPosition() const {
	return m_position;
}

Actor::vec3_t *Actor::GetPositionPtr() {
	return &m_position;
}
*/

/*
types::Matrix44 & Actor::GetScaleMatrix() {
	return m_matrices.scale;
}

types::Matrix44 & Actor::GetTranslateMatrix() {
	return m_matrices.translate;
}

types::Matrix44 & Actor::GetRotateMatrix() {
	return m_matrices.rotate;
}

void Actor::UpdateScale() {
	m_matrices.scale.TransformScale( m_scale.x, m_scale.y, m_scale.z );
}

void Actor::UpdatePosition() {
	m_matrices.translate.TransformTranslate( m_position.x, m_position.y, m_position.z );
}

void Actor::UpdateRotation() {
	m_matrices.rotate.TransformRotate( m_angle.x, m_angle.y, m_angle.z );
}
*/

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
	UpdateWorldMatrix();
}

void Actor::SetScene( Scene *scene ) {
	if ( m_scene != NULL && scene != NULL )
		throw ActorError( "duplicate scene set" );
	m_scene = scene;
}

Scene *Actor::GetScene() {
	return m_scene;
}

} /* namespace actor */
} /* namespace scene */
