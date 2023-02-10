#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

class Mesh;
class Image;

namespace shader_program {

CLASS( Orthographic, ShaderProgram )

	Orthographic() : ShaderProgram( TYPE_ORTHO ) {};
	
protected:
	friend class opengl::Mesh;
	friend class opengl::Image;

	struct {
		GLuint texture;
		GLuint world;
		GLuint light_pos;
		GLuint light_color;
		GLuint flags;
		GLuint tint_color;
	} uniforms;

	struct {
		GLuint coord;
		GLuint tex_coord;
		GLuint tint_color;
		GLuint normal;
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

