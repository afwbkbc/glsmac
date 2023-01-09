#include "OrthographicOpenGLShaderProgram.h"

namespace renderer {
namespace opengl {
namespace shader_program {

void OrthographicOpenGLShaderProgram::AddShaders() {
	this->AddShader( GL_VERTEX_SHADER, "#version 130 \n\
\
in vec2 aCoord; \
in vec2 aTexCoord; \
uniform float uZIndex; \
out vec2 texpos; \
\
void main(void) { \
	gl_Position = vec4( aCoord, uZIndex, 1.0 ); \
	texpos = vec2(aTexCoord); \
} \
\
");

	this->AddShader( GL_FRAGMENT_SHADER, "#version 130 \n\
\
uniform vec4 uTint; \
uniform sampler2D uSampler; \
out vec4 FragColor; \
in vec2 texpos; \
\
void main(void) { \
	/*FragColor = vec4(texture2D(uSampler, vec2(texpos.st))); */\
	FragColor = uTint; \
} \
\
");

}

void OrthographicOpenGLShaderProgram::Initialize() {
	//m_gl_uniforms.sampler = this->GetUniformLocation( "uSampler" );
	m_gl_uniforms.z_index = this->GetUniformLocation( "uZIndex" );
	m_gl_uniforms.tint = this->GetUniformLocation( "uTint" );
	m_gl_attributes.coord = this->GetAttributeLocation( "aCoord" );
	//m_gl_attributes.tex_coord = this->GetAttributeLocation( "aTexCoord" );
};

void OrthographicOpenGLShaderProgram::EnableAttributes() const {
	glEnableVertexAttribArray( m_gl_attributes.coord );
	glVertexAttribPointer( m_gl_attributes.coord, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)0 );
	//glEnableVertexAttribArray( m_gl_attributes.tex_coord );
	//glVertexAttribPointer( m_gl_attributes.tex_coord, 2, GL_FLOAT, GL_FALSE, 4, (const GLvoid *)2 );
};

void OrthographicOpenGLShaderProgram::DisableAttributes() const {
	glDisableVertexAttribArray( m_gl_attributes.coord );
	//glDisableVertexAttribArray( m_gl_attributes.tex_coord );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace renderer */
