#pragma once

#include <string>
#include <GL/glew.h>

#include "base/Module.h"

namespace renderer {
namespace opengl {
namespace shader_program {

MAJOR_CLASS( OpenGLShaderProgram, base::Module )
	enum type_t {
		TYPE_ORTHO,
		TYPE_FONT,
		TYPE_PERSP,
		TYPE_SKYBOX,
	};
	OpenGLShaderProgram( const type_t type ) : m_type( type ) {};
	const type_t GetType() const {
		return m_type;
	}

	void Start();
	void Stop();
	void Enable();
	void Disable();
protected:
	const type_t m_type;

	virtual void AddShaders() {};
	virtual void Initialize() {};
	virtual void EnableAttributes() const {};
	virtual void DisableAttributes() const {};

	void AddShader( GLenum type, const std::string data );
	GLint GetUniformLocation( const std::string name );
	GLint GetAttributeLocation( const std::string name );
	void BindAttribLocation( GLuint index,const std::string name );

	bool m_enabled = false;
	GLuint m_gl_shader_program = 0;
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace renderer */
