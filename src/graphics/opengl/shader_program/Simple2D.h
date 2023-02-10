#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

class Mesh;
class Image;

namespace shader_program {

CLASS( Simple2D, ShaderProgram )
	Simple2D() : ShaderProgram( TYPE_SIMPLE2D ) {};
protected:
	friend class opengl::Mesh;
	friend class opengl::Image;

	struct {
		GLint texture;
	} uniforms;

	struct {
		GLint coord;
		GLint tex_coord;
	} attributes;

	void AddShaders();
	void Initialize();
	void EnableAttributes() const;
	void DisableAttributes() const;
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */

#include "../actor/Mesh.h"
