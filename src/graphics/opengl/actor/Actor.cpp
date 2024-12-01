#include "Actor.h"

#include "scene/actor/Actor.h"

namespace graphics {
namespace opengl {

Actor::Actor( OpenGL* opengl, scene::actor::Actor* actor )
	: m_opengl( opengl )
	, m_actor( actor ) {
	m_name = actor->GetLocalName();
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
