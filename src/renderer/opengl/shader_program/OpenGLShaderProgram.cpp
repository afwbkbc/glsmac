#include <GL/glew.h>

#include "OpenGLShaderProgram.h"

namespace renderer {
namespace opengl {
namespace shader_program {

void OpenGLShaderProgram::Start() {

	Log( "Adding shaders" );

	m_gl_shader_program = glCreateProgram();
	if ( !m_gl_shader_program )
		throw OpenGLShaderProgramError( "Unable to create shader program!" );

	this->AddShaders();

	glLinkProgram( m_gl_shader_program );

	GLint success = 0;

	glGetProgramiv( m_gl_shader_program, GL_LINK_STATUS, &success );
	if ( !success )
		throw OpenGLShaderProgramError( "Shader program linking failed!" );

	glValidateProgram( m_gl_shader_program );
	glGetProgramiv( m_gl_shader_program, GL_VALIDATE_STATUS, &success );
	if ( !success )
		throw OpenGLShaderProgramError( "Invalid shader program!" );

	glUseProgram( m_gl_shader_program );

	Initialize();

	m_enabled = false;
}

void OpenGLShaderProgram::Stop() {

	Log("Removing shaders");

	glDeleteProgram( m_gl_shader_program );

}

void OpenGLShaderProgram::AddShader( GLenum type, const std::string data ) {

	GLuint gl_shader = glCreateShader( type );
	if ( !gl_shader )
		throw OpenGLShaderProgramError("Error creating OpenGL shader!");

	const GLchar* p[1];
	p[0] = data.c_str();
	GLint Lengths[1];
	Lengths[0]= data.size();
	glShaderSource( gl_shader, 1, p, Lengths );

	glCompileShader( gl_shader );
	GLint success;
	glGetShaderiv( gl_shader, GL_COMPILE_STATUS, &success );
	if ( !success ) {
		GLint info_log_length;
		glGetShaderiv( gl_shader, GL_INFO_LOG_LENGTH, &info_log_length );
		GLchar* str_info_log = new GLchar[info_log_length + 1];
		glGetShaderInfoLog( gl_shader, info_log_length, NULL, str_info_log );

		throw OpenGLShaderProgramError("Error compiling shader! shader data: '"+data+"' ; shader error: '"+str_info_log+"'");

		delete str_info_log;

	}

	glAttachShader( m_gl_shader_program, gl_shader );

}

GLint OpenGLShaderProgram::GetUniformLocation( const std::string name ) {
	GLuint uniform = glGetUniformLocation( m_gl_shader_program, name.c_str() );
	if ( uniform == 0xFFFFFFFF )
		throw OpenGLShaderProgramError( "Unable to get uniform location of '"+name+"'!" );
	return uniform;
}

GLint OpenGLShaderProgram::GetAttributeLocation( const std::string name ) {
	GLuint attribute = glGetAttribLocation( m_gl_shader_program, name.c_str() );
	if ( attribute == 0xFFFFFFFF )
		throw OpenGLShaderProgramError( "Unable to get attribute location of '"+name+"'!" );
	return attribute;
}

void OpenGLShaderProgram::Enable() {
	if ( !m_enabled ) {

		this->EnableAttributes();

		glUseProgram( m_gl_shader_program );

		m_enabled = true;
	}
}

void OpenGLShaderProgram::Disable() {
	if ( m_enabled ) {

		this->DisableAttributes();

		glUseProgram(0);

		m_enabled = false;
	}
}

void OpenGLShaderProgram::BindAttribLocation( GLuint index,const std::string name ) {
	glBindAttribLocation( m_gl_shader_program, index, name.c_str() );
}

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace renderer */
