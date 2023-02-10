#pragma once

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {

class Mesh;

namespace shader_program {

CLASS( OrthographicData, ShaderProgram )

	OrthographicData() : ShaderProgram( TYPE_ORTHO_DATA ) {};
	
protected:
	friend class opengl::Mesh;

	struct {
		GLint world;
	} uniforms;

	struct {
		GLint coord;
		GLint data;
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

