#include "Actor.h"

using namespace types;

namespace graphics {
namespace opengl {

Actor::Actor( scene::actor::Actor *actor ) : m_actor( actor ) {
	m_name = actor->GetLocalName();
	m_position = actor->GetPosition();
}

const Vec3& Actor::GetPosition() const {
	return m_position;
}

void Actor::SetPosition( const Vec3& position ) {
	m_position = position;
}

Actor::~Actor() {

}

} /* namespace opengl */
} /* namespace graphics */
