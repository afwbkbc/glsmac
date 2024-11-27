#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

class Sprite;

class Mesh;

namespace shader_program {

CLASS( Orthographic, ShaderProgram )

	Orthographic()
		: ShaderProgram( TYPE_ORTHO ) {};

protected:
	friend class opengl::Sprite;
	friend class opengl::Mesh;

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

	void AddShaders() override;
	void Initialize() override;
	void EnableAttributes() const override;
	void DisableAttributes() const override;

	void ClearFlags() override;
};

}
}
}
