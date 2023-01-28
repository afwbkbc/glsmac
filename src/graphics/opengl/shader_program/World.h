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
	World() : ShaderProgram( TYPE_PERSP ) {};

	types::Matrix44 GetWorldMatrix();
protected:
	friend class opengl::Mesh;
	friend class routine::World;

	struct {
		GLint world;
		GLint light_color;
		GLint light_intensity;
		GLint campos;
	} m_gl_uniforms;

	struct {
		GLint position;
		GLint tex_coord;
	} m_gl_attributes;

	void AddShaders();
	void Initialize();
	void EnableAttributes() const;
	void DisableAttributes() const;
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */

#include "../actor/Mesh.h"
#include "../routine/World.h"