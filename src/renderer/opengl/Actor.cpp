#include "Actor.h"

namespace renderer {
namespace opengl {

Actor::Actor( scene::actor::Actor *actor ) : m_actor( actor ) {
	m_name = actor->GetLocalName();
}

Actor::~Actor() {

}

} /* namespace opengl */
} /* namespace renderer */
