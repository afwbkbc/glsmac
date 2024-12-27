#pragma once

#include "Actor.h"

namespace graphics {
namespace opengl {
namespace actor {

CLASS( CachingActor, Actor )
protected:
	virtual void UpdateImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr ) = 0;
};

}
}
}