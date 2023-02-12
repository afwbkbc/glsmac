#pragma once

#include "scene/actor/InstancedMesh.h"

namespace game {
namespace world {
namespace actor {

CLASS( Actor, scene::actor::InstancedMesh )
	
	Actor( const std::string& name );
	
	virtual void Iterate() {}

};

}
}
}
