#pragma once

#include <GL/glew.h>

#include "Actor.h"

#include "types/Vec2.h"

namespace scene::actor {
class Cache;
}

namespace graphics {
namespace opengl {

CLASS( Cache, Actor )

	Cache( OpenGL* opengl, scene::actor::Cache* actor );
	~Cache();

protected:
	void DrawImpl( shader_program::ShaderProgram* shader_program, scene::Camera* camera = nullptr ) override;

};

}
}
