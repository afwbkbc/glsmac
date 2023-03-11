#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

class Text;

namespace shader_program {

CLASS( Font, ShaderProgram )
	Font() : ShaderProgram( TYPE_FONT ) {};
protected:
	friend class opengl::Text;

	struct {
		GLuint flags;
		GLuint position;
		GLuint texture;
		GLuint color;
		GLuint z_index;
		struct {
			GLuint min;
			GLuint max;
		} area_limits;
	} uniforms;

	struct {
		GLuint coord;
	} attributes;

	void AddShaders();
	void Initialize();
	void EnableAttributes() const;
	void DisableAttributes() const;

};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */

#include "../actor/Text.h"
