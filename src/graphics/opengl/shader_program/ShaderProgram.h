#pragma once

#include <string>
#include <GL/glew.h>

#include "common/Module.h"

namespace graphics {
namespace opengl {
namespace shader_program {

CLASS( ShaderProgram, common::Module )
	enum type_t {
		TYPE_SIMPLE2D,
		TYPE_ORTHO,
		TYPE_ORTHO_DATA,
		TYPE_FONT,
		TYPE_PERSP,
		TYPE_SKYBOX,
	};
	ShaderProgram( const type_t type )
		: m_type( type ) {};
	const type_t GetType() const {
		return m_type;
	}

	void Start() override;
	void Stop() override;
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
	void BindAttribLocation( GLuint index, const std::string name );

	bool m_enabled = false;
	GLuint m_gl_shader_program = 0;

	// shader helpers
	const std::string S_HasFlag( const std::string& var, const GLuint flag ) const;
	const std::string S_For( const std::string& iterator, const size_t begin, const size_t end, const std::string& body ) const;

};

}
}
}
