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
in vec2 texpos; \
uniform sampler2D uTexture; \
uniform vec4 uTint; \
out vec4 FragColor; \
\
void main(void) { \
	FragColor = vec4(texture2D(uTexture, vec2(texpos.xy)).xyz + uTint.xyz, uTint.w); \
	/*FragColor = uTint;*/ \
} \
\
");

}

void OrthographicOpenGLShaderProgram::Initialize() {
	m_gl_attributes.tex_coord = this->GetAttributeLocation( "aTexCoord" );
	m_gl_uniforms.texture = this->GetUniformLocation( "uTexture" );
	m_gl_uniforms.z_index = this->GetUniformLocation( "uZIndex" );
	m_gl_uniforms.tint = this->GetUniformLocation( "uTint" );
	m_gl_attributes.coord = this->GetAttributeLocation( "aCoord" );
};

void OrthographicOpenGLShaderProgram::EnableAttributes() const {
	glEnableVertexAttribArray( m_gl_attributes.coord );
	glVertexAttribPointer( m_gl_attributes.coord, 2, GL_FLOAT, GL_FALSE, 8, (const GLvoid *)0 );
	glEnableVertexAttribArray( m_gl_attributes.tex_coord );
	glVertexAttribPointer( m_gl_attributes.tex_coord, 2, GL_FLOAT, GL_FALSE, 8, (const GLvoid *)8 );
};

void OrthographicOpenGLShaderProgram::DisableAttributes() const {
	glDisableVertexAttribArray( m_gl_attributes.coord );
	glDisableVertexAttribArray( m_gl_attributes.tex_coord );
};

} /* namespace shader_program */
} /* namespace opengl */
} /* namespace renderer */
