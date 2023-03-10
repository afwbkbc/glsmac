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
		GLuint texture;
		GLuint flags;
		struct {
			GLuint min;
			GLuint max;
		} coordinate_limits;
	} uniforms;

	struct {
		GLuint coord;
		GLuint tex_coord;
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
