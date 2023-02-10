#include <GL/glew.h>

#include "ShaderProgram.h"

namespace graphics {
namespace opengl {
namespace shader_program {

void ShaderProgram::Start() {

	Log( "Adding shaders" );

	m_gl_shader_program = glCreateProgram();
	ASSERT( m_gl_shader_program, "Unable to create shader program!" );

	AddShaders();

	glLinkProgram( m_gl_shader_program );

	GLint success = 0;

	glGetProgramiv( m_gl_shader_program, GL_LINK_STATUS, &success );
	ASSERT( success, "Shader program linking failed!" );

	glValidateProgram( m_gl_shader_program );
	glGetProgramiv( m_gl_shader_program, GL_VALIDATE_STATUS, &success );
	ASSERT( success, "Invalid shader program!" );

	glUseProgram( m_gl_shader_program );

	Initialize();

	m_enabled = false;
}

void ShaderProgram::Stop() {

	Log("Removing shaders");

	glDeleteProgram( m_gl_shader_program );

}

void ShaderProgram::AddShader( GLenum type, const std::string data ) {

	GLuint gl_shader = glCreateShader( type );
	ASSERT( gl_shader, "Error creating OpenGL shader!" );

	const GLchar* p[1];
	p[0] = data.c_str();
	GLint Lengths[1];
	Lengths[0]= data.size();
	glShaderSource( gl_shader, 1, p, Lengths );

	glCompileShader( gl_shader );
	GLint success;
	glGetShaderiv( gl_shader, GL_COMPILE_STATUS, &success );
	
#ifdef DEBUG
	if ( !success ) {
		GLint info_log_length;
		glGetShaderiv( gl_shader, GL_INFO_LOG_LENGTH, &info_log_length );
		GLchar* str_info_log = (GLchar*) malloc( info_log_length + 1 );
		glGetShaderInfoLog( gl_shader, info_log_length, NULL, str_info_log );

		THROW( "Error compiling shader! shader data: '" + data + "' ; shader error: '" + str_info_log + "'" );

		free( str_info_log );

	}
#endif

	glAttachShader( m_gl_shader_program, gl_shader );

}

GLint ShaderProgram::GetUniformLocation( const std::string name ) {
	GLuint uniform = glGetUniformLocation( m_gl_shader_program, name.c_str() );
	ASSERT( uniform != 0xFFFFFFFF, "Unable to get uniform location of '" + name + "'!" );
	return uniform;
}

GLint ShaderProgram::GetAttributeLocation( const std::string name ) {
	GLuint attribute = glGetAttribLocation( m_gl_shader_program, name.c_str() );
	ASSERT( attribute != 0xFFFFFFFF, "Unable to get attribute location of '" + name + "'!" );
	return attribute;
}

void ShaderProgram::Enable() {
	if ( !m_enabled ) {

		EnableAttributes();

		glUseProgram( m_gl_shader_program );

		m_enabled = true;
	}
}

void ShaderProgram::Disable() {
	if ( m_enabled ) {

		DisableAttributes();

		glUseProgram(0);

		m_enabled = false;
	}
}

void ShaderProgram::BindAttribLocation( GLuint index,const std::string name ) {
	glBindAttribLocation( m_gl_shader_program, index, name.c_str() );
}

const string ShaderProgram::S_HasFlag( const string& var, const GLuint flag ) const {
	return "( ( " + var + " & uint( " + to_string( flag ) + " ) ) == uint( " + to_string( flag ) + " ) )";
}

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace graphics */
