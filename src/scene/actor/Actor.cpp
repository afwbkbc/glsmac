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
	return m_scene;
}

} /* namespace actor */
} /* namespace scene */
