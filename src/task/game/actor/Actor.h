#pragma once

#include "scene/actor/Mesh.h"

namespace task {
namespace game {
namespace actor {

CLASS( Actor, scene::actor::Mesh )

	Actor( const std::string& name );

	virtual void Iterate() {}

};

}
}
}
