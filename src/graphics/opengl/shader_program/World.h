#pragma once

#include "ShaderProgram.h"

#include "types/Matrix44.h"

namespace graphics {
namespace opengl {

namespace routine {
class World;
}

class Mesh;

namespace shader_program {

CLASS( World, ShaderProgram )
	World()
		: ShaderProgram( TYPE_PERSP ) {};

	types::Matrix44 GetWorldMatrix();
protected:
	friend class opengl::Mesh;
	friend class routine::World;

	struct {
		GLint world;
		GLint light_color;
		GLint light_intensity;
		GLint campos;
	} uniforms;

	struct {
		GLint position;
		GLint tex_coord;
	} attributes;

	void AddShaders() override;
	void Initialize() override;
	void EnableAttributes() const override;
	void DisableAttributes() const override;
};

}
}
}
