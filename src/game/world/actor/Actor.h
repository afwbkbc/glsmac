#pragma once

#include "scene/actor/Mesh.h"

namespace game {
namespace world {
namespace actor {

CLASS( Actor, scene::actor::Mesh )
	
	Actor( const std::string& name );
	
	virtual void Iterate() {}

};

}
}
}
