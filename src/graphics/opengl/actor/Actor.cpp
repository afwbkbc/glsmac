#include "Actor.h"

using namespace types;

namespace graphics {
namespace opengl {

Actor::Actor( scene::actor::Actor* actor )
	: m_actor( actor ) {
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

} /* namespace opengl */
} /* namespace graphics */
