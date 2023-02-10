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
		GLint texture;
		GLint color;
		GLint z_index;
	} uniforms;

	struct {
		GLint coord;
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
