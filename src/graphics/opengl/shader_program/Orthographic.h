#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

class Sprite;
class Mesh;
class Text;
class FBO;
class Cache;

namespace shader_program {

CLASS( Orthographic, ShaderProgram )

	Orthographic()
		: ShaderProgram( TYPE_ORTHO ) {};

protected:
	friend class opengl::Sprite;
	friend class opengl::Mesh;
	friend class opengl::Text;
	friend class opengl::FBO;
	friend class opengl::Cache;

	struct {
		GLuint position;
		GLuint texture;
		GLuint world;
		GLuint instances;
		GLuint light_pos;
		GLuint light_color;
		GLuint flags;
		GLuint tint_color;
		struct {
			GLuint min;
			GLuint max;
		} area_limits;
	} uniforms;

	struct {
		GLuint coord;
		GLuint tex_coord;
		GLuint tint_color;
		GLuint normal;
	} attributes;

protected:
	void AddShaders() override;
	void Initialize() override;
	void EnableAttributes() const override;
	void DisableAttributes() const override;
};

}
}
}
