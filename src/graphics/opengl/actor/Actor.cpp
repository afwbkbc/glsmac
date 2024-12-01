#include "Actor.h"

#include "scene/actor/Actor.h"
#include "scene/actor/Cache.h"
#include "common/ObjectLink.h"

#include <iostream>

namespace graphics {
namespace opengl {

Actor::Actor( OpenGL* opengl, scene::actor::Actor* actor )
	: m_opengl( opengl )
	, m_actor( actor ) {
	m_name = actor->GetLocalName();
}

void Actor::Draw( shader_program::ShaderProgram* shader_program, scene::Camera* camera ) {
	if ( !m_actor->GetCacheParent() ) { // cache parent manages draws of children itself
		DrawImpl( shader_program, camera );
	}
}

const float Actor::GetZIndex() const {
	return m_z_index;
}

void Actor::SetZIndex( const float z_index ) {
	m_z_index = z_index;
}

Actor::~Actor() {

}

}
}
